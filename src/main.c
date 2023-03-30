#include <stdio.h>

#include "image_handler.h"

int main(int argc, char* argv[]) {
    IH_render_init();

    struct IH_Image image;
    IH_import_image(&image, "../../samples/qoi/testcard.qoi", IH_QOI);

    printf("width: %d\n", image.width);
    printf("height: %d\n", image.height);
    printf("encoding: %d\n", image.encoding);
    printf("channels: %d\n", image.channels);
    printf("colorspace: %d\n", image.colorspace);
    IH_render_image(&image);

    IH_delete_image(&image);
    IH_render_terminate();

    return 0;
}
