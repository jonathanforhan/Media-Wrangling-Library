#ifndef IMAGE_HANDLER_QOI_H
#define IMAGE_HANDLER_QOI_H

#include <stdio.h>
#include <stdint.h>
#include "image_handler.h"

/*
 * QOI IMAGE SPEC
 * https://qoiformat.org/qoi-specification.pdf
 */

IH_Image *IH_qoi_to_raw(FILE *fptr, uint64_t file_length);

#endif //IMAGE_HANDLER_QOI_H
