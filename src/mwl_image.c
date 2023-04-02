#include <mwl_image.h>

#include "qoi.h"

#define MWL_ABORT abort
#include "util.h"

#define MWL_QOI_MAGIC (*(uint32_t*)("qoif"))
#define MWL_PNG_MAGIC (*(uint64_t*)(uint8_t[]){ 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a })

MWL_Image *MWL_import_image(const char *path) {
    uint8_t *buffer = NULL;
    MWL_Image *image = NULL;

    FILE *fptr = fopen(path, "rb");
    MWL_ASSERT(fptr != NULL);
    
    fseek(fptr, 0, SEEK_END);
    uint64_t file_len = (uint64_t)ftell(fptr);
    MWL_ASSERT(file_len != 0 && file_len != (uint64_t)-1);
    rewind(fptr);

    MWL_ASSERT(buffer = malloc(file_len));

    // if fails, not all bytes read
    MWL_ASSERT(fread(buffer, sizeof(uint8_t), file_len, fptr) == file_len);
    fclose(fptr);
    fptr = NULL;

    if (*(uint32_t*)buffer == MWL_QOI_MAGIC) {
        MWL_ASSERT(image = MWL_qoi_to_raw(buffer, file_len));
    }
    else if (*(uint64_t*)buffer == MWL_PNG_MAGIC) {
        printf("PNG TODO\n\n");
    }

    return image;

abort:
    MWL_LOG_ABORT();
    if (fptr != NULL)
        fclose(fptr);
    free(buffer);
    return NULL;
}

void MWL_free_image(MWL_Image *image) {
    if (image != NULL) {
        free(image->data);
    }
    free(image);
}

int MWL_export_image(const MWL_Image *image, enum MWL_image_type type, const char *path) {
    MWL_ASSERT(image != NULL);
    MWL_ASSERT(image->data != NULL);

    switch (type) {
    case MWL_QOI:
        return MWL_export_raw_to_qoi(image, path);
    case MWL_PNG:
        //return MWL_export_raw_to_png(image, path);
        return MWL_FAILURE;
    case MWL_JPG:
        //return MWL_export_raw_to_jpg(image, path);
        return MWL_FAILURE;
    }
    fprintf(stderr, "\033[31mFile-type error\n\033[0m");

abort:
    MWL_LOG_ABORT();
    return MWL_FAILURE;
}

