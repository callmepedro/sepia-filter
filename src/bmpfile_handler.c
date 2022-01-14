#include "../include/bmpfile_handler.h"
#include "../include/bmp_header.h"
#include <stdbool.h>
#include <stdlib.h>


#define BF_TYPE 19778
#define BF_RESERVED 0
#define BI_SIZE 40
#define BI_PLANES 1
#define BI_BIT_COUNT 24
#define BI_COMPRESSION 0

static uint8_t padding_value(uint64_t width) {
    return 4 - (width * 3) % 4;
}

static struct bmp_header create_bmp_header(uint32_t width, uint32_t height) {
    uint32_t size_image = height * (width * 3 + padding_value(width));
    return (struct bmp_header) {
            .bfType = BF_TYPE,
            .bfReserved = BF_RESERVED,
            .bOffBits = sizeof(struct bmp_header),
            .biSize = BI_SIZE,
            .biWidth = width,
            .biHeight = height,
            .biPlanes = BI_PLANES,
            .biBitCount = BI_BIT_COUNT,
            .biCompression = BI_COMPRESSION,
            .biSizeImage = size_image,
            .bfileSize = size_image + sizeof (struct bmp_header),
            .biXPelsPerMeter = 0,
            .biYPelsPerMeter = 0,
            .biClrUsed = 0,
            .biClrImportant = 0,
    };
}

static bool byte_array_from_img(struct image const* img, uint8_t* byte_array) {
    if (img == NULL || byte_array == NULL) {
        fprintf(stderr, "Invalid argument(s): %s\n", __FUNCTION__);
        return false;
    }
    size_t arr_pointer = 0;
    for (size_t i = 0; i < img->height; ++i) {
        for (size_t j = 0; j < img->width; ++j) {
            byte_array[arr_pointer] = (img->data)[i*img->width+j].b;
            arr_pointer++;
            byte_array[arr_pointer] = (img->data)[i*img->width+j].g;
            arr_pointer++;
            byte_array[arr_pointer] = (img->data)[i*img->width+j].r;
            arr_pointer++;
        }
        for (size_t i = 0; i < padding_value(img->width); ++i) {
            byte_array[arr_pointer] = 0;
            arr_pointer++;
        }
    }
    return true;
}

static bool img_from_byte_array(struct image* img, uint8_t const* byte_array) {
    size_t arr_pointer = 0;
    for (size_t i = 0; i < img->height; ++i) {
        for (size_t j = 0; j < img->width; ++j) {
            (img->data)[i*img->width+j].b = byte_array[arr_pointer];
            arr_pointer++;
            (img->data)[i*img->width+j].g = byte_array[arr_pointer];
            arr_pointer++;
            (img->data)[i*img->width+j].r = byte_array[arr_pointer];
            arr_pointer++;
        }
        arr_pointer += padding_value(img->width);
    }
    return true;
}

enum read_status from_bmp( FILE* in, struct image* img ) {
    struct bmp_header header = {0};
    if (fread(&header, sizeof(struct bmp_header), 1, in) != 1) {
        fprintf(stderr, "Header reading failed: %s\n", __FUNCTION__);
        return READ_INVALID_HEADER;
    }
    if (header.biBitCount != 24) {
        fprintf(stderr, "Program works only with 24bit-bmp\n");
        return READ_INVALID_SIGNATURE;
    }
    img->width = header.biWidth;
    img->height = header.biHeight;
    img->data = malloc(sizeof(struct pixel) * img->width * img->height);

    size_t byte_array_size = (img->width * 3 + padding_value(img->width)) * img->height;
    uint8_t* byte_array = malloc(sizeof(uint8_t) * byte_array_size);
    if (byte_array == NULL) {
        fprintf(stderr, "Memory allocation for byte_array failed: %s\n", __FUNCTION__);
        return READ_ERROR;
    }

    if (fread(byte_array, sizeof(uint8_t), byte_array_size, in) != byte_array_size) {
        fprintf(stderr, "byte_array reading failed: %s\n", __FUNCTION__);
        return READ_INVALID_BITS;
    }

    if (!img_from_byte_array(img, byte_array)) {
        return READ_ERROR;
    }
    free(byte_array);

    return READ_OK;
}

enum write_status to_bmp( FILE* out, struct image const* img ) {

    struct bmp_header header = create_bmp_header(img->width, img->height);

    size_t byte_array_size = (img->width*3 + padding_value(img->width)) * img->height;
    uint8_t* byte_array = malloc(sizeof(uint8_t) * byte_array_size);
    if (byte_array == NULL) {
        fprintf(stderr, "Memory allocation for byte_array failed: %s\n", __FUNCTION__);
        return WRITE_ERROR;
    }
    if (!byte_array_from_img(img, byte_array)) {
        free(byte_array);
        return WRITE_INVALID_HEADER;
    }
    if (fwrite(&header, header.bOffBits, 1, out) != 1) {
        free(byte_array);
        fprintf(stderr, "Header writing failed: %s\n", __FUNCTION__);
        return WRITE_INVALID_HEADER;
    }

    if (fwrite(byte_array, sizeof(uint8_t), byte_array_size, out) != byte_array_size) {
        free(byte_array);
        fprintf(stderr, "byte_array writing failed: %s\n", __FUNCTION__);
        return WRITE_INVALID_BITS;
    }
    free(byte_array);

    return WRITE_OK;
}


