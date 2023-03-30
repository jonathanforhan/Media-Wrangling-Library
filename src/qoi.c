#include "qoi.h"

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

/*
 * source:
 * https://github.com/phoboslab/qoi
 */

#define QOI_HEADER_SIZE 14
#define QOI_OP_INDEX  0x00 /* 00xxxxxx */
#define QOI_OP_DIFF   0x40 /* 01xxxxxx */
#define QOI_OP_LUMA   0x80 /* 10xxxxxx */
#define QOI_OP_RUN    0xc0 /* 11xxxxxx */
#define QOI_OP_RGB    0xfe /* 11111110 */
#define QOI_OP_RGBA   0xff /* 11111111 */

#define QOI_MASK_2    0xc0 /* 11000000 */
static const uint8_t QOI_PADDING[8] = { 0, 0, 0, 0, 0, 0, 0, 1 };

typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } rgba;
    uint32_t v;
} qoi_pixel;

#define QOI_HASH(C) ((C.rgba.r * 3 + C.rgba.g * 5 + C.rgba.b * 7 + C.rgba.a * 11) % 64)

void IH_qoi_to_raw(struct IH_Image *image, FILE *fptr, uint64_t file_length) {
    uint8_t *buffer = malloc(file_length); // free w/ IH_delete_image;
    fread(buffer, sizeof(uint8_t), file_length, fptr);

    const char *header = (char*)buffer;
    uint8_t *qoi_data = &buffer[QOI_HEADER_SIZE];

    if(strncmp(header, "qoif", 4) != 0) {
        perror("INVALID QOI FORMAT");
        free(buffer);
        image = NULL;
        return;
    }

    // QOI width and height are stored as big endian
    image->width = ntohl(*(int*)&header[4]);
    image->height = ntohl(*(int*)&header[8]);
    image->encoding = 255;
    image->colorspace = (uint8_t)header[13];
    image->channels = (uint8_t)header[12];
    image->type = IH_QOI;

    // header complete, parse body //

    uint32_t raw_data_len = image->width * image->height * image->channels;
    uint8_t *raw_data = malloc(raw_data_len);

    qoi_pixel index[64] = { 0 };
    qoi_pixel px;

    px.rgba.r = 0;
    px.rgba.g = 0;
    px.rgba.b = 0;
    px.rgba.a = 255;

    size_t p = 0, run = 0;
    for(size_t i = 0; i < raw_data_len; i += image->channels) {
        if (run > 0) {
            --run;
            goto data_transfer;
        }

        int byte = qoi_data[p++];

        if (byte == QOI_OP_RGB) {
            px.rgba.r = qoi_data[p++];
            px.rgba.g = qoi_data[p++];
            px.rgba.b = qoi_data[p++];
        }
        else if (byte == QOI_OP_RGBA) {
            px.rgba.r = qoi_data[p++];
            px.rgba.g = qoi_data[p++];
            px.rgba.b = qoi_data[p++];
            px.rgba.a = qoi_data[p++];
        }
        else if ((byte & QOI_MASK_2) == QOI_OP_INDEX) {
            px = index[byte];
        }
        else if ((byte & QOI_MASK_2) == QOI_OP_DIFF) {
            px.rgba.r += ((byte >> 4) & 0x03) - 2;
            px.rgba.g += ((byte >> 2) & 0x03) - 2;
            px.rgba.b += ( byte       & 0x03) - 2;
        }
        else if ((byte & QOI_MASK_2) == QOI_OP_LUMA) {
            int32_t vg = (byte & 0x3f) - 32;
            byte = qoi_data[p++];
            px.rgba.r += vg - 8 + ((byte >> 4) & 0x0f);
            px.rgba.g += vg;
            px.rgba.b += vg - 8 +  (byte       & 0x0f);
        }
        else if ((byte & QOI_MASK_2) == QOI_OP_RUN) {
            run = (byte & 0x3f);
        }

        index[QOI_HASH(px)] = px;

        data_transfer:
        raw_data[i + 0] = px.rgba.r;
        raw_data[i + 1] = px.rgba.g;
        raw_data[i + 2] = px.rgba.b;

        if (image->channels == IH_RGBA) {
            raw_data[i + 3] = px.rgba.a;
        }
    }
    free(buffer);

    image->data = raw_data;
}
