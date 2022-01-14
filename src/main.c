#include <stdio.h>
#include "../include/image_manage.h"
#include "../include/bmpfile_handler.h"
#include "../include/image_sepia.h"

int main(int argc, char** argv) {

    if (argc != 3) {
        fprintf(stderr, "2 arguments expected\n");
        return 1;
    }
    FILE* in = fopen(argv[1], "rb");
    if (in == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return 1;
    }
    struct image img = {0};
    from_bmp(in, &img);
    fclose(in);

    sepia_inplace(&img);

    FILE* out = fopen(argv[2], "wb");
    if (out == NULL) {
        fprintf(stderr, "Error opening output file\n");
    }
    to_bmp(out, &img);
    fclose(out);
    img_destroy(&img);

    return 0;
}
