#include <stdio.h>
#include <stdlib.h>
#include <math.h>  // for nan()
#include <errno.h>
#include "image.h"
#include "./png.h"

static size_t allocate_data(struct image *img)
{
	size_t size = img->width * img->height * sizeof(img->data[0]);
	img->data = malloc(size);
	return size;
}

static int image_set_pixel(struct image *img, int x, int y, uint32_t color)
{
	if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
		// Coordinates out of bounds
		errno = EINVAL;
		return 1;
	}

	size_t index = y * img->width + x;
	img->data[index] = color;
	return 0;
}

int image_load(struct image *img, FILE *fp)
{
	int (*load_func)(struct image *, FILE *);

	if (is_png(fp)) {
		fprintf(stderr, "Loading PNG...");
		load_func = png_load;
	} else {
		fprintf(stderr, "Unsupported file type.\n");
		exit(-ENOTSUP);
	}

	if (load_func(img, fp) != 0) {
		fprintf(stderr, "Image loading failed\n");
		return 1;
	}

	fprintf(stderr, "loaded\n");
	fprintf(stderr, "Size %d x %d\n", img->width, img->height);
	return 0;
}

uint32_t image_get_pixel(const struct image *img, int x, int y)
{
	if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
		// Coordinates out of bounds
		errno = EINVAL;
		return 0;
	}

	size_t index = y * img->width + x;
	return img->data[index];
}

void image_downscale(struct image *dst, const struct image *src, int factorx, int factory)
{
	dst->width = (src->width + factorx - 1) / factorx; //round up
	dst->height = (src->height + factory - 1) / factory; //round up
	allocate_data(dst);

	for (int dstx = 0; dstx < dst->width; ++dstx) {
		int srcx = dstx * factorx;
		for (int dsty = 0; dsty < dst->height; ++dsty) {
			int srcy = dsty * factory;
			unsigned int total_r = 0, total_g = 0, total_b = 0;
			unsigned char avg_r, avg_g, avg_b;
			int pixels_counted = 0;

			for (int xi = 0; xi < factorx && srcx + xi < src->width; ++xi) {
				for (int yi = 0; yi < factory && srcy + yi < src->height; ++yi) {
					uint32_t pixel = image_get_pixel(src, srcx + xi, srcy + yi);
					if (alpha(pixel) != 0) {
						total_r += red(pixel);
						total_g += green(pixel);
						total_b += blue(pixel);
						++pixels_counted;
					}
				}
			}
		
			uint32_t new_pixel;
			if (pixels_counted == 0) {
				new_pixel = 0;
			} else {
				avg_r = total_r / pixels_counted;
				avg_g = total_g / pixels_counted;
				avg_b = total_b / pixels_counted;
				new_pixel = rgba(avg_r, avg_g, avg_b, 255);
			}
			image_set_pixel(dst, dstx, dsty, new_pixel);
		}
	}
}

uint32_t rgba(unsigned char r, unsigned char g, unsigned char b,
		unsigned char a)
{
	return (a << 24) | (b << 16) | (g << 8) | r;
}

unsigned char red(uint32_t pixel)
{
	return pixel & 0xff;
}

unsigned char green(uint32_t pixel)
{
	return (pixel & 0xff00) >> 8;
}

unsigned char blue(uint32_t pixel)
{
	return (pixel & 0xff0000) >> 16;
}

unsigned char alpha(uint32_t pixel)
{
	return (pixel & 0xff000000) >> 24;
}
