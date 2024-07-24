#ifndef DISPLAY_H
#define DISPLAY_H

#include "image.h"

void display_window(const struct image *img,
		int x, int y, int dx, int dy,
		int pixel_width, int zoom);

#endif
