#ifndef IMAGE_HANDLER_IMAGE_HANDLER_H
#define IMAGE_HANDLER_IMAGE_HANDLER_H

typedef char IH_int8;
typedef short IH_int16;
typedef long IH_int32;
typedef long long IH_int64;
typedef unsigned char IH_uint8;
typedef unsigned short IH_uint16;
typedef unsigned long IH_uint32;
typedef unsigned long long IH_uint64;

enum IH_channels { IH_RGB = 3, IH_RGBA = 4 };

enum IH_colorspace { IH_sRGB = 0, IH_linear = 1 };

enum IH_image_type {
  IH_PPM,
  IH_JPG,
  IH_PNG,
  IH_QOI,
};

enum IH_color_type { IH_TODO_color_type };

enum IH_compression_method { IH_TODO_compression_method };

enum IH_filter_method { IH_TODO_filter_method };

enum IH_interlace_method { IH_TODO_interlace_method };

typedef struct IH_Image {
  IH_uint32 width;  // width in pixels
  IH_uint32 height; // height in pixels
  IH_uint8 encoding;
  enum IH_channels channels;
  enum IH_colorspace colorspace;
  enum IH_image_type type;

  IH_uint8 bit_depth;
  enum IH_color_type color_type;
  enum IH_compression_method compression_method;
  enum IH_filter_method filter_method;
  enum IH_interlace_method interlace_method;

  IH_uint8 *data; // image data

} IH_Image;

void IH_render_init(void);

void IH_render_terminate(void);

IH_Image *IH_import_image(const char *path, enum IH_image_type type);

void IH_render_image(IH_Image *image);

void IH_delete_image(IH_Image *image);

#endif // IMAGE_HANDLER_IMAGE_HANDLER_H
