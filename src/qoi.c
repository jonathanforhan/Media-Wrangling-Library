#include "qoi.h"

#define MWL_ABORT abort
#include "util.h"

/*
 * source:
 * https://github.com/phoboslab/qoi
 */

#define MWL_QOI_HEADER_SIZE 14
#define MWL_QOI_OP_INDEX  0x00 /* 00xxxxxx */
#define MWL_QOI_OP_DIFF   0x40 /* 01xxxxxx */
#define MWL_QOI_OP_LUMA   0x80 /* 10xxxxxx */
#define MWL_QOI_OP_RUN    0xc0 /* 11xxxxxx */
#define MWL_QOI_OP_RGB    0xfe /* 11111110 */
#define MWL_QOI_OP_RGBA   0xff /* 11111111 */

#define MWL_QOI_MASK_2    0xc0 /* 11000000 */

#define MWL_QOI_MAGIC   (*(uint32_t*)("qoif"))
#define MWL_QOI_PADDING (*(uint64_t*)(uint8_t[8]){ 0, 0, 0, 0, 0, 0, 0, 1 })
#define MWL_QOI_HASH(C) ((C.rgba.r * 3 + C.rgba.g * 5 + C.rgba.b * 7 + C.rgba.a * 11) % 64)

typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } rgba;
    uint32_t v;
} mwl_qoi_pixel;

// QOI image format to raw pixel data in Image sruct
MWL_Image *MWL_qoi_to_raw(uint8_t *qoi_buf, uint32_t file_len) {
    uint8_t *raw_buf = NULL;
    MWL_Image *image = NULL;
    MWL_ASSERT(file_len > MWL_QOI_HEADER_SIZE);

    MWL_ASSERT(image = malloc(sizeof(MWL_Image)));

    image->width = MWL_ntohl(*(uint32_t*)&qoi_buf[4]);
    image->height = MWL_ntohl(*(uint32_t*)&qoi_buf[8]);
    image->channels = (uint8_t)qoi_buf[12];

    MWL_ASSERT((int)image->width > 0 && (int)image->height > 0);

    uint32_t raw_buf_len = image->width * image->height * image->channels;
    MWL_ASSERT(raw_buf = malloc(raw_buf_len));

    mwl_qoi_pixel index[64] = { 0 };
    mwl_qoi_pixel px = {{ 0, 0, 0, 255 }}; /* r g b a */

    size_t p = MWL_QOI_HEADER_SIZE, run = 0;
    for(size_t i = 0; i < raw_buf_len; i += image->channels) {
        if (run > 0) {
            --run;
            goto data_transfer;
        }

        MWL_ASSERT((p) < file_len);
        int byte = qoi_buf[p++];

        if (byte == MWL_QOI_OP_RGB) {
            MWL_ASSERT((p+2) < file_len);
            px.rgba.r = qoi_buf[p++];
            px.rgba.g = qoi_buf[p++];
            px.rgba.b = qoi_buf[p++];
        }
        else if (byte == MWL_QOI_OP_RGBA) {
            MWL_ASSERT((p+3) < file_len);
            px.rgba.r = qoi_buf[p++];
            px.rgba.g = qoi_buf[p++];
            px.rgba.b = qoi_buf[p++];
            px.rgba.a = qoi_buf[p++];
        }
        else if ((byte & MWL_QOI_MASK_2) == MWL_QOI_OP_INDEX) {
            px = index[byte];
        }
        else if ((byte & MWL_QOI_MASK_2) == MWL_QOI_OP_DIFF) {
            px.rgba.r += ((byte >> 4) & 0x03) - 2;
            px.rgba.g += ((byte >> 2) & 0x03) - 2;
            px.rgba.b += ( byte       & 0x03) - 2;
        }
        else if ((byte & MWL_QOI_MASK_2) == MWL_QOI_OP_LUMA) {
            MWL_ASSERT((p) < file_len);
            int32_t vg = (byte & 0x3f) - 32;
            byte = qoi_buf[p++];
            px.rgba.r += vg - 8 + ((byte >> 4) & 0x0f);
            px.rgba.g += vg;
            px.rgba.b += vg - 8 +  (byte       & 0x0f);
        }
        else if ((byte & MWL_QOI_MASK_2) == MWL_QOI_OP_RUN) {
            run = (byte & 0x3f);
        }

        index[MWL_QOI_HASH(px)] = px;

data_transfer:
        raw_buf[i + 0] = px.rgba.r;
        raw_buf[i + 1] = px.rgba.g;
        raw_buf[i + 2] = px.rgba.b;

        if (image->channels == MWL_RGBA) {
            raw_buf[i + 3] = px.rgba.a;
        }
    } // end for loop

    image->data = raw_buf;
    image->data_len = raw_buf_len;
    return image;

abort:
    MWL_LOG_ABORT();
    free(raw_buf);
    free(image);
    return NULL;
}

// MWL_Image exported as qoi
int MWL_export_raw_to_qoi(const MWL_Image *image, const char *path) {
    FILE *fptr = NULL;
    uint8_t *qoi_buf = NULL;

    // maximum possible size of qoi image
    uint32_t qoi_buf_len = image->width * image->height * image->channels
        + MWL_QOI_HEADER_SIZE + sizeof(MWL_QOI_PADDING);
    MWL_ASSERT(qoi_buf = malloc(qoi_buf_len));

    *(uint32_t*)qoi_buf = MWL_QOI_MAGIC;
    *(uint32_t*)&qoi_buf[4] = MWL_htonl(image->width);
    *(uint32_t*)&qoi_buf[8] = MWL_htonl(image->height);
	qoi_buf[12] = image->channels;
	qoi_buf[13] = 0;

    size_t p = MWL_QOI_HEADER_SIZE, run = 0;

    mwl_qoi_pixel index[64] = { 0 };
    mwl_qoi_pixel px = {{ 0, 0, 0, 255 }};
    mwl_qoi_pixel px_prev;

	uint32_t raw_buf_len = image->width * image->height * image->channels;
	uint32_t px_end = raw_buf_len - image->channels;

    const uint8_t *raw_buf = image->data;

	for (size_t i = 0; i < raw_buf_len; i += image->channels) {
		px_prev = px;
		px.rgba.r = raw_buf[i + 0];
		px.rgba.g = raw_buf[i + 1];
		px.rgba.b = raw_buf[i + 2];

		if (image->channels == 4) {
			px.rgba.a = raw_buf[i + 3];
		}

		if (px.v == px_prev.v) {
			++run;
			if (run == 62 || i == px_end) {
				qoi_buf[p++] = MWL_QOI_OP_RUN | (run - 1);
				run = 0;
			}
		    continue;
		}

		if (run > 0) {
			qoi_buf[p++] = MWL_QOI_OP_RUN | (run - 1);
			run = 0;
		}

		size_t index_pos = MWL_QOI_HASH(px);

		if (index[index_pos].v == px.v) {
			qoi_buf[p++] = MWL_QOI_OP_INDEX | index_pos;
		    continue;
		}
		index[index_pos] = px;

		if (px.rgba.a == px_prev.rgba.a) {
			int8_t vr = px.rgba.r - px_prev.rgba.r;
			int8_t vg = px.rgba.g - px_prev.rgba.g;
			int8_t vb = px.rgba.b - px_prev.rgba.b;

			int8_t vg_r = vr - vg;
			int8_t vg_b = vb - vg;

            // MWL_WITHIN: (X, A, B) -> (X > A && X < B)
			if (MWL_WITHIN(vr, -3, 2) && MWL_WITHIN(vg, -3, 2) && MWL_WITHIN(vb, -3, 2)) {
				qoi_buf[p++] = MWL_QOI_OP_DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2);
			}
			else if (MWL_WITHIN(vg_r, -9, 8) && MWL_WITHIN(vg, -33, 32) && MWL_WITHIN(vg_b, -9, 8)) {
				qoi_buf[p++] = MWL_QOI_OP_LUMA | (vg   + 32);
				qoi_buf[p++] = (vg_r + 8) << 4 | (vg_b +  8);
			}
			else {
				qoi_buf[p++] = MWL_QOI_OP_RGB;
				qoi_buf[p++] = px.rgba.r;
				qoi_buf[p++] = px.rgba.g;
				qoi_buf[p++] = px.rgba.b;
			}
		}
		else {
			qoi_buf[p++] = MWL_QOI_OP_RGBA;
			qoi_buf[p++] = px.rgba.r;
			qoi_buf[p++] = px.rgba.g;
			qoi_buf[p++] = px.rgba.b;
			qoi_buf[p++] = px.rgba.a;
		}
	}

	*(uint64_t*)&qoi_buf[p] = MWL_QOI_PADDING;
	p += sizeof(MWL_QOI_PADDING); // EOF

    fptr = fopen(path, "wb");
    MWL_ASSERT(fptr != NULL);

    MWL_ASSERT(fwrite(qoi_buf, sizeof(uint8_t), p, fptr));
    fclose(fptr);
    free(qoi_buf);

    return MWL_SUCCESS;

abort:
    MWL_LOG_ABORT();
    if (fptr != NULL) {
        fclose(fptr);
    }
    free(qoi_buf);
    return MWL_FAILURE;
}
