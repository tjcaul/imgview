#include <stdio.h>
#include "display.h"

void display_window(const struct image *img,
		int x, int y, int dx, int dy,
		int pixel_width, int zoom)
{
	// Clear and home screen
	printf("\033[2J\033[H");
	fflush(stdout);

	printf("\033[7m %d x %d  (/%d zoom) \033[0m\n", img->width, img->height, zoom);
	for (int yi = 0; yi < dy && y + yi < img->height; ++yi) {
		for (int xi = 0; xi < dx && x + xi < img->width; ++xi) {
			uint32_t pixel = image_get_pixel(img, x + xi, y + yi);
			int r = red(pixel), g = green(pixel), b = blue(pixel);
			printf("\033[48;2;%hhu;%hhu;%hhum", r, g, b);
			for (int i = 0; i < pixel_width; ++i)
				putchar(' ');
		}
		printf("\033[0m\n");
	}
}
