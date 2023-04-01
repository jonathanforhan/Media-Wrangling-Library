#include "qoi.h"

#include <stdlib.h>
#include <string.h>
 
#include "util.h"

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

#define QOI_MAGIC   (*(uint32_t*)("qoid"))
#define QOI_PADDING   (*(uint64_t*)(uint8_t[8]){ 0, 0, 0, 0, 0, 0, 0, 1 })
#define QOI_HASH(C) ((C.rgba.r * 3 + C.rgba.g * 5 + C.rgba.b * 7 + C.rgba.a * 11) % 64)

#define QOI_ASSERT(X) if(!(X)) { goto abort; }

typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } rgba;
    uint32_t v;
} qoi_pixel;

IH_Image *IH_qoi_to_raw(FILE *fptr, uint64_t file_length) {
    uint8_t *buffer = NULL, *raw_data = NULL;
    IH_Image *image = NULL;

    image = malloc(sizeof(IH_Image));
    QOI_ASSERT(image != NULL);

    buffer = malloc(file_length);
    QOI_ASSERT(buffer != NULL);

    fread(buffer, sizeof(uint8_t), file_length, fptr);
    QOI_ASSERT(*(uint32_t*)buffer == QOI_MAGIC);

    // QOI width and height are stored as big endian
    image->width = be_to_le(*(uint32_t*)&buffer[4]);
    image->height = be_to_le(*(uint32_t*)&buffer[8]);
    image->encoding = 255;
    image->colorspace = (uint8_t)buffer[13];
    image->channels = (uint8_t)buffer[12];
    image->type = IH_QOI;

    QOI_ASSERT(image->width != 0 && image->height != 0);

    // not used by QOI
    image->bit_depth = 0;
    image->color_type = 0;
    image->compression_method = 0;
    image->filter_method = 0;
    image->interlace_method = 0;

    // header complete, parse body //

    uint32_t raw_data_len = image->width * image->height * image->channels;
    raw_data = malloc(raw_data_len);
    QOI_ASSERT(raw_data != NULL);

    qoi_pixel index[64] = { 0 };
    qoi_pixel px = {{ 0, 0, 0, 255 }}; /* r b g a */

    size_t p = QOI_HEADER_SIZE, run = 0;
    for(size_t i = 0; i < raw_data_len; i += image->channels) {
        if (run > 0) {
            --run;
            goto data_transfer;
        }

        int byte = buffer[p++];

        if (byte == QOI_OP_RGB) {
            px.rgba.r = buffer[p++];
            px.rgba.g = buffer[p++];
            px.rgba.b = buffer[p++];
        }
        else if (byte == QOI_OP_RGBA) {
            px.rgba.r = buffer[p++];
            px.rgba.g = buffer[p++];
            px.rgba.b = buffer[p++];
            px.rgba.a = buffer[p++];
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
            byte = buffer[p++];
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
    } // end for loop
    free(buffer);
    image->data = raw_data;
    return image;

abort:
    if(buffer != NULL)
        perror("INVALID QOI FORMAT");
    else
        perror("OUT OF MEMORY");
    free(raw_data);
    free(buffer);
    free(image);
    IH_backtrace();
    return NULL;
}
