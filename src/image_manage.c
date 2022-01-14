#include "../include/image_manage.h"
#include <stdlib.h>

struct image img_create(uint64_t width, uint64_t height){
    return (struct image) {
            .width = width,
            .height = height,
            .data = malloc(sizeof(struct pixel) * width * height)
    };
}

void img_destroy(struct image* img){
    free(img->data);
}
