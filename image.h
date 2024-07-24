#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdint.h>

struct image {
	int width;
	int height;
	uint32_t *data;
};

int image_load(struct image *img, FILE *fp);
uint32_t image_get_pixel(const struct image *img, int x, int y);
void image_downscale(struct image *dst, const struct image *src, int factorx, int factory);
uint32_t rgba(unsigned char r, unsigned char g, unsigned char b,
		unsigned char a);
unsigned char red(uint32_t pixel);
unsigned char green(uint32_t pixel);
unsigned char blue(uint32_t pixel);
unsigned char alpha(uint32_t pixel);

#endif
