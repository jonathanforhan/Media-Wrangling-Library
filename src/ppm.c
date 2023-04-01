#include "ppm.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

IH_Image *IH_ppm_to_raw(FILE *fptr, uint64_t file_length) {
    bool p6 = false;
    uint32_t width = 0, height = 0, encoding = 0;
    char line[1024];
    while(fgets(line, 1024, fptr)) {
        if(encoding != 0 && height != 0 && width != 0) {
            // if incomplete fields, truncate comments
            const char *trunc = strchr(line, '#');
            if(trunc != NULL) {
                int i = (int)(trunc - &line[0]);
                line[i] = '\0';
            }
        }

        char *token = strtok(line, " ");
        while(token != NULL) {
            if(strcmp(token, "P6") != 0 && p6 == false)
                p6 = true;
            else if(!width)
                width = atoi(token);
            else if(!height)
                height = atoi(token);
            else if(!encoding)
                encoding = atoi(token);

            token = width * height * encoding == 0 ? strtok(NULL, " ") : NULL;
        }
        if(encoding != 0 && height != 0 && width != 0)
            break;
    }
    if(p6 == false) {
        printf("Image Handler only accepts P6 encoded .ppm files\n");
        return NULL;
    }

    uint8_t *data = calloc(file_length, sizeof(uint8_t)); // free w/ IH_delete_image;
    fread(data, sizeof(uint8_t), file_length, fptr);

    IH_Image *image = malloc(sizeof(IH_Image));
    if(image == NULL) return NULL;

    image->width = width;
    image->height = height;
    image->encoding = (uint8_t)encoding;
    image->channels = IH_RGB;
    image->colorspace = IH_sRGB;
    image->type = IH_PPM;
    image->data = data;

    return image;
}
