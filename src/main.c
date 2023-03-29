#include "qoi.h"
#include "ppm.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    import_image("../../samples/sign_1.ppm", PPM);
    return 0;
}
