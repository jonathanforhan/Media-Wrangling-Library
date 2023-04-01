#include "png.h"

#include <stdlib.h>
#include <stdio.h>

#include "util.h"

#define PNG_MAGIC (*(IH_uint64*)(IH_uint8[8]){0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a})
// Critical chunks
#define PNG_IHDR (*(IH_uint32*)("IHDR"))
#define PNG_PLTE (*(IH_uint32*)("PLTE"))
#define PNG_IDAT (*(IH_uint32*)("IDAT"))
#define PNG_IEND (*(IH_uint32*)("IEND"))
// Ancillary chunks
#define PNG_bKGD (*(IH_uint32*)("bKGD"))
#define PNG_cHRM (*(IH_uint32*)("cHRM"))
#define PNG_gAMA (*(IH_uint32*)("gAMA"))
#define PNG_hIST (*(IH_uint32*)("hIST"))
#define PNG_pHYs (*(IH_uint32*)("pHYs"))
#define PNG_sBIT (*(IH_uint32*)("sBIT"))
#define PNG_tEXt (*(IH_uint32*)("tEXt"))
#define PNG_tIME (*(IH_uint32*)("tIME"))
#define PNG_tRNS (*(IH_uint32*)("tRNS"))
#define PNG_zTXt (*(IH_uint32*)("zTXt"))

#define PNG_ASSERT(X) if(!(X)) { goto abort; }

void IH_IHDR_chunk(IH_Image *image, IH_uint8 *buffer, size_t *p) {
    image->width = be_to_le(*(IH_uint32*)&buffer[*p]);
    *p += 4;

    image->height = be_to_le(*(IH_uint32*)&buffer[*p]);
    *p += 4;

    image->bit_depth = buffer[*p++];

    image->encoding = 255;
    image->channels = (IH_uint8)buffer[12];
    image->type = IH_PNG;

    image->bit_depth = 0;
    image->color_type = 0;
    image->compression_method = 0;
    image->filter_method = 0;
    image->interlace_method = 0;

}

IH_Image *IH_png_to_raw(void *fptr, IH_uint64 file_length) {
    IH_uint8 *buffer = NULL, *raw_data = NULL;
    IH_Image *image = NULL;

    image = malloc(sizeof(IH_Image));
    PNG_ASSERT(image != NULL);

    buffer = malloc(file_length);
    PNG_ASSERT(buffer != NULL);

    fread(buffer, sizeof(IH_uint8), file_length, fptr);
    PNG_ASSERT(*(IH_uint64*)buffer == PNG_MAGIC);

    size_t p = sizeof(PNG_MAGIC);

    IH_uint32 chunk_len = be_to_le(*(IH_uint32*)&buffer[p]);
    printf("%lu\n", chunk_len);
    p += 4;

    PNG_ASSERT(*(IH_uint32*)&buffer[p] == PNG_IHDR);

    IH_uint32 tag = *(IH_uint32*)&buffer[p];
    p += 4;
    if (tag == PNG_IHDR) {
        IH_IHDR_chunk(image, buffer, &p);
    }
    else if (tag == PNG_IDAT) {
        
    }
    else if (tag == PNG_IEND) {

    }
    else {
        goto abort;
    }


    free(buffer);
    return image;

abort:
    if(buffer != NULL)
        perror("INVALID PNG FORMAT");
    else
        perror("OUT OF MEMORY");
    free(raw_data);
    free(buffer);
    free(image);
    IH_backtrace();
    return NULL;
}
