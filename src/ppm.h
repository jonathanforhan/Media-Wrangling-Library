#ifndef IMAGE_HANDLER_PPM_H
#define IMAGE_HANDLER_PPM_H

#include <stdio.h>
#include <stdint.h>
#include "image_handler.h"

/*
 * PPM IMAGE SPEC, NOTE: ONLY P6 SUPPORTED
 * https://people.computing.clemson.edu/~dhouse/courses/405/notes/ppm-files.pdf
 */

IH_Image *IH_ppm_to_raw(FILE *fptr, uint64_t file_length);

#endif //IMAGE_HANDLER_PPM_H
