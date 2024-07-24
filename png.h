#ifndef IMGVIEW_PNG_H
#define IMGVIEW_PNG_H

#include <stdio.h>
#include <stdbool.h>
#include "image.h"

bool is_png(FILE *fp);
int png_load(struct image *img, FILE *fp);

#endif
