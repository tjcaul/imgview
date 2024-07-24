#include <png.h>
#include <stdlib.h>
#include "./png.h"

#define PNG_SIG_BYTES_TO_CHECK 8

static bool is_supported(int bit_depth, int color_type)
{
	if (bit_depth != 8)
		return false;
	if (!(color_type == PNG_COLOR_TYPE_RGBA || color_type == PNG_COLOR_TYPE_RGB))
		return false;
	return true;
}

bool is_png(FILE *fp)
{
	char *sig;
	size_t siglen;
	bool return_value;

	sig = malloc(PNG_SIG_BYTES_TO_CHECK);
	siglen = fread(sig, 1, PNG_SIG_BYTES_TO_CHECK, fp);
	if (siglen != PNG_SIG_BYTES_TO_CHECK)
		return false;
	rewind(fp);

	return_value = !png_sig_cmp(sig, 0, siglen);
	free(sig);
	return return_value;
}

int png_load(struct image *img, FILE *fp)
{
	png_struct *png_struct;
	png_info *png_info;
	unsigned char **row_pointers;
	int bit_depth, color_type;

	png_struct = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	if (!png_struct)
		return 1;

	png_info = png_create_info_struct(png_struct);
	if (!png_info)
		return 1;

	// Set up png error handling
	if (setjmp(png_jmpbuf(png_struct))) {
		png_destroy_read_struct(&png_struct, &png_info, NULL);
		return 1;
	}

	// Read info
	png_init_io(png_struct, fp);
	png_read_info(png_struct, png_info);
	png_get_IHDR(png_struct, png_info, NULL, NULL, &bit_depth, &color_type,
			NULL, NULL, NULL);

	// Add alpha channel if missing
	if (color_type == PNG_COLOR_TYPE_RGB)
		png_set_filler(png_struct, 0xff, PNG_FILLER_AFTER);

	if (!is_supported(bit_depth, color_type)) {
		fprintf(stderr, "Only 8-bit RGBA is supported.\n");
		png_destroy_read_struct(&png_struct, &png_info, NULL);
		return 1;
	}

	img->width = png_get_image_width(png_struct, png_info);
	img->height = png_get_image_height(png_struct, png_info);
	img->data = malloc(img->width * img->height * sizeof(img->data[0]));

	// Set up pointers into the buffer
	row_pointers = malloc(img->height * sizeof(row_pointers[0]));
	for (int i = 0; i < img->height; ++i) {
		size_t offset = i * img->width * sizeof(img->data[0]);
		row_pointers[i] = (unsigned char *)img->data + offset;
	}

	// Read image data
	png_read_image(png_struct, row_pointers);

	// Clean up
	free(row_pointers);
	png_destroy_read_struct(&png_struct, &png_info, NULL);
	return 0;
}
