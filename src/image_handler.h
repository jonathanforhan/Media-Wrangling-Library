#ifndef IMAGE_HANDLER_IMAGE_HANDLER_H
#define IMAGE_HANDLER_IMAGE_HANDLER_H

#include <stdint.h>
#include <pthread.h>

typedef pthread_t IH_render_handle;
typedef pthread_t IH_handle;

enum IH_channels {
    IH_RGB = 3,
    IH_RGBA = 4
};

enum IH_colorspace {
    IH_sRGB = 0,
    IH_linear = 1
};

enum IH_image_type {
    IH_PPM,
    IH_JPG,
    IH_PNG,
    IH_QOI,
};

struct IH_Image {
    uint32_t width;   // width in pixels
    uint32_t height;  // height in pixels
    uint8_t encoding;
    enum IH_channels channels;
    enum IH_colorspace colorspace;
    enum IH_image_type type;

    uint8_t *data;       // image data
};

void IH_render_init(void);

void IH_render_terminate(void);

void IH_import_image(struct IH_Image *image, const char *path, enum IH_image_type type);

void IH_render_image(struct IH_Image *image);

void IH_delete_image(struct IH_Image *image);

#endif //IMAGE_HANDLER_IMAGE_HANDLER_H
