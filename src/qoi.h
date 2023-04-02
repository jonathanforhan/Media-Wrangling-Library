#ifndef MWL_SRC_QOI_H
#define MWL_SRC_QOI_H

#include <mwl_image.h>

/*
 * QOI IMAGE SPEC
 * https://qoiformat.org/qoi-specification.pdf
 */

MWL_Image *MWL_qoi_to_raw(uint8_t *qoi_buf, uint32_t file_len);

int MWL_export_raw_to_qoi(const MWL_Image *image, const char *path);

#endif // MWL_SRC_QOI_H
