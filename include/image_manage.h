#ifndef IMAGE_MANAGE_H
#define IMAGE_MANAGE_H

#include <stdbool.h>
#include "image.h"
#include <inttypes.h>


struct image img_create(uint64_t width, uint64_t height);

void img_destroy(struct image* img);

#endif
