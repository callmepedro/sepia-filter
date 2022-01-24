#ifndef IMAGE_SEPIA_H
#define IMAGE_SEPIA_H

#include <inttypes.h>
#include "image.h"
#include <stdlib.h>

void sepia_c_inplace(struct image* img);

void sepia_asm_inplace(struct image* img);

#endif
