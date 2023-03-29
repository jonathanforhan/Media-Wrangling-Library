#include <stdio.h>

#include "qoi.h"
#include "ppm.h"

int main(int argc, char* argv[]) {
    IH_init();

    struct IH_Image image;
    IH_import_image(&image, "../../samples/flowers.ppm", IH_PPM);

    IH_render_handle handle = IH_render_image(&image);
    printf("width: %d, height: %d, encoding: %d\n", image.width, image.height, image.encoding);

    IH_join_handle(handle);
    IH_delete_image(&image);

    return 0;
}
