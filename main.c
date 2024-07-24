#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "image.h"
#include "display.h"


#define PIXEL_WIDTH 1
#define PIXEL_RATIO 2

void print_usage(const char *progname)
{
	fprintf(stderr, "Usage: %s <file> [zoom]\n", progname);
}

int main(int argc, char **argv)
{
	FILE *fp;
	struct image image;
	int zoom;
	struct winsize winsize;
	int screen_width, screen_height;

	if (argc < 2 || argc > 3) {
		print_usage(argv[0]);
		exit(1);
	}

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);
	screen_width = winsize.ws_col;
	screen_height = winsize.ws_row - 2; //-2 for header and footer
	fprintf(stderr, "Screen %d x %d\n", screen_width, screen_height);

	fp = fopen(argv[1], "rb");

	if (!fp) {
		perror("fopen");
		exit(-errno);
	}
	
	if (image_load(&image, fp))
		return 1;

	if (argc == 3) {
		zoom = atoi(argv[2]);
	} else {
		int min_zoom_width = (image.width + screen_width - 1) / screen_width; //round up
		int min_zoom_height = (image.height / PIXEL_RATIO + screen_height - 1) / screen_height; //round up
		zoom = min_zoom_width > min_zoom_height ? min_zoom_width : min_zoom_height;
	}

	struct image image2;
	image_downscale(&image2, &image, zoom, zoom * PIXEL_RATIO);

	display_window(&image2, 0, 0, screen_width, screen_height, PIXEL_WIDTH, zoom);

	return 0;
}
