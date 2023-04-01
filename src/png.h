#ifndef IMAGE_HANDLER_PNG_H
#define IMAGE_HANDLER_PNG_H

#include "./image_handler.h"

/*
 * PNG IMAGE SPEC
 * https://www.w3.org/TR/2022/WD-png-3-20221025/
 */

IH_Image *IH_png_to_raw(void *fptr, IH_uint64 file_length);

#endif  // IMAGE_HANDLER_PNG_H
