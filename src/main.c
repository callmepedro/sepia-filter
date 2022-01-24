#include <stdio.h>
#include <sys/resource.h>
#include "string.h"
#include "../include/image_manage.h"
#include "../include/bmpfile_handler.h"
#include "../include/image_sepia.h"

static long sepia_c_test(struct image* img) {
    struct rusage r;
    struct timeval start;
    struct timeval end;

    getrusage(RUSAGE_SELF, &r );
    start = r.ru_utime;

    sepia_c_inplace(img);

    getrusage(RUSAGE_SELF, &r );
    end = r.ru_utime;
    long res = ((end.tv_sec - start.tv_sec) * 1000000L) +
                 end.tv_usec - start.tv_usec;

    return res;
}

static long sepia_asm_test(struct image* img) {
    struct rusage r;
    struct timeval start;
    struct timeval end;

    getrusage(RUSAGE_SELF, &r );
    start = r.ru_utime;

    sepia_asm_inplace(img);

    getrusage(RUSAGE_SELF, &r );
    end = r.ru_utime;
    long res = ((end.tv_sec - start.tv_sec) * 1000000L) +
               end.tv_usec - start.tv_usec;

    return res;
}

static bool read_img(struct image* img, char* path) {
    FILE* in = fopen(path, "rb");
    if (in == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return false;
    }
    from_bmp(in, img);
    fclose(in);
    return true;
}

static bool write_img(struct image* img, char* path) {
    FILE* out = fopen(path, "wb");
    if (out == NULL) {
        fprintf(stderr, "Error opening output file\n");
        return false;
    }
    to_bmp(out, img);
    fclose(out);
    img_destroy(img);
    return true;
}

#define TEST_ITER 50

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "3 arguments expected\n");
        return 1;
    }

    if (strcmp(argv[3], "asm") == 0) {
        struct image img = {0};
        if (!read_img(&img, argv[1])) {
            return 1;
        }
        sepia_asm_inplace(&img);
        if (!write_img(&img, argv[2])) {
            return 1;
        }
    }

    else if (strcmp(argv[3], "c") == 0) {
        struct image img = {0};
        if (!read_img(&img, argv[1])) {
            return 1;
        }
        sepia_c_inplace(&img);
        if (!write_img(&img, argv[2])) {
            return 1;
        }
    }

    else if (strcmp(argv[3], "test") == 0) {
        long c_time = 0;
        long asm_time = 0;
        for (size_t i = 0; i < TEST_ITER; ++i) {
            struct image img1 = {0};
            if (!read_img(&img1, argv[1])) {
                return 1;
            }
            c_time += sepia_c_test(&img1);
            if (!write_img(&img1, argv[2])) {
                return 1;
            }

            struct image img2 = {0};
            if (!read_img(&img2, argv[1])) {
                return 1;
            }
            asm_time += sepia_asm_test(&img2);
            if (!write_img(&img2, argv[2])) {
                return 1;
            }
        }
        c_time /= TEST_ITER;
        asm_time /= TEST_ITER;

        printf("sepia_c: Time elapsed in microseconds: %ld\n", c_time);
        printf("sepia_asm: Time elapsed in microseconds: %ld\n", asm_time);
    }

    else {
        fprintf(stderr, "Invalid mode\n");
        return 1;
    }

    return 0;
}
