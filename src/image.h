#ifndef IMAGE_HANDLER_IMAGE_H
#define IMAGE_HANDLER_IMAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

enum channel {
    RGB = 3,
    RGBA = 4
};

enum colorspace {
    sRGB,
    linear
};

enum image_type {
    PPM,
    JPG,
    PNG,
    QOI,
};

struct Image {
    uint32_t width;   // width in pixels
    uint32_t height;  // height in pixels
    enum channel;
    enum colorspace;
    enum image_type;

    uint8_t* data;    // image data
};

uint8_t* import_image(const char* path, enum image_type type) {
    FILE *fptr;
    uint8_t* file_buffer;

    fptr = fopen(path, "r");
    if(fptr == NULL)
        perror("FILE READ ERROR");

    fseek(fptr, 0L, SEEK_END);
    uint64_t file_length = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    bool p6 = false;
    uint32_t width = 0, height = 0;
    char line[1024];
    while(fgets(line, 1024, fptr) != NULL) {
        size_t i = 0;
        while(i < 1024 && line[i] != '\0') {
            if(line[i] == '#')
                break;
            if(line[i] == 'P' && line[i+1] == '6') {
                p6 = true;
                i += 2;
                continue;
            }
            char* digit = calloc(32, sizeof(char));
            while(isdigit(line[i]) && digit[31] == '\0') {
                strcat(digit, &line[i++]);
            }
            if (width == 0) {
                width = atoi(digit);
            } else {
                height = atoi(digit);
            }
            free(digit);

            ++i;
        }
        if(width * height * p6 != 0)
            break;
    }

    printf("width: %d, height: %d, P6: %d", width, height, p6);

    file_buffer = (uint8_t*)calloc(file_length, sizeof(uint8_t));

    if(file_buffer == NULL)
        perror("FILE BUFFER ERROR");

    fread(file_buffer, sizeof(uint8_t), file_length, fptr);
    fclose(fptr);

}

#endif //IMAGE_HANDLER_IMAGE_H
