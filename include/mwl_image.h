#ifndef MWL_INCLUDE_MWL_IMAGE_H
#define MWL_INCLUDE_MWL_IMAGE_H

#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <stdio.h>
#include <stdlib.h>

enum MWL_image_type {
    MWL_QOI,
    MWL_PNG,
    MWL_JPG,
};

enum MWL_channels {
    MWL_RGB = 3,
    MWL_RGBA = 4,
};

typedef struct MWL_Image {
    uint32_t width;
    uint32_t height;
    enum MWL_channels channels;

    uint8_t *data;
} MWL_Image;

// Import qoi, png, or jpg image
MWL_Image *MWL_import_image(const char *path);

// Free image and contents.
// This is the only safe way to free image
void MWL_free_image(MWL_Image *image);

// Export image as specified type
void MWL_export_image(MWL_Image *image, enum MWL_image_type type, const char *path);

#endif // MWL_INCLUDE_MWL_IMAGE_H
