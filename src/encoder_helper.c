#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ezpng.h"
#include "program_settings.h"
#include "rgb_bits.h"
#include "rgbaf.h"

RGBAF* get_png_to_bin_setup(ProgramSettings* settings)
{
	const ezpng_rgba* input_image;
	RGBAF* working_image;
	ezpng_decoder* input_file = ezpng_decoder_open(settings->input_file_path);

	if (!input_file)
	{
		ezpng_print_error("Error opening PNG input file");
		return NULL;
	}

	if (settings->verbose)
		puts("Getting data from PNG");

	input_image = ezpng_decoder_get_data(input_file);

	if (!input_image)
	{
		ezpng_print_error("Error getting PNG input image");
		goto cleanup;
	}

	if (settings->verbose)
		puts("Getting width and height");

	settings->width = ezpng_decoder_get_width(input_file);
	settings->height = ezpng_decoder_get_height(input_file);

	if (settings->verbose)
		puts("Calculating resolution");

	settings->resolution = settings->width * settings->height;

	working_image = malloc(settings->resolution * sizeof(*working_image));

	if (!working_image)
	{
		seprint("Unable to allocate memory for working image");
		goto cleanup;
	}

	if (settings->verbose)
		puts("Allocated memory for working image");

	for (int i = 0; i < settings->resolution; ++i)
	{
		working_image[i].r = (float)input_image[i].r;
		working_image[i].g = (float)input_image[i].g;
		working_image[i].b = (float)input_image[i].b;
		working_image[i].a = (float)input_image[i].a;
	}

	if (settings->verbose)
		puts("Finished casting input image to working image");

cleanup:
	if (input_file)
		ezpng_decoder_close(input_file);

	return working_image;
}

int get_png_to_bin_quantize(ProgramSettings* settings, RGBAF* working_image)
{
	int rc = 1;
	int direction = 1;

	if (settings->verbose)
		puts("Initiating quantization loop");

	for (int y = 0; y < settings->height; ++y)
	{
		int x = direction == 1 ? 0 : settings->width - 1;

		for (int i = 0; i < settings->width; ++i)
		{
			RGBAF* current_pixel =
			    RGBAF_get(working_image, settings->width, settings->height, x, y);

			if (!current_pixel)
			{
				fprintf(stderr, "FATAL: unable to get current pixel. (%i, %i)\n", x, y);
				rc = 0;
				goto cleanup;
			}

			RGBAF oldpixel = RGBAF_copy(current_pixel);
			RGBAF newpixel = find_closest_limited_color(current_pixel, false);
			newpixel = promote_pixel(&newpixel, false);
			*current_pixel = newpixel;

			// skip dithering
			if (!settings->dither)
				continue;

			RGBAF quant_error = RGBAF_subtract(&oldpixel, &newpixel);

			if (direction == 1)
			{
				apply_dither(working_image, &quant_error, 7.f / 16.f, settings->width,
				             settings->height, x + 1, y, settings->dither_alpha);
				apply_dither(working_image, &quant_error, 3.f / 16.f, settings->width,
				             settings->height, x - 1, y + 1, settings->dither_alpha);
				apply_dither(working_image, &quant_error, 5.f / 16.f, settings->width,
				             settings->height, x, y + 1, settings->dither_alpha);
				apply_dither(working_image, &quant_error, 1.f / 16.f, settings->width,
				             settings->height, x + 1, y + 1, settings->dither_alpha);
			}
			else
			{
				apply_dither(working_image, &quant_error, 7.f / 16.f, settings->width,
				             settings->height, x - 1, y, settings->dither_alpha);
				apply_dither(working_image, &quant_error, 3.f / 16.f, settings->width,
				             settings->height, x + 1, y + 1, settings->dither_alpha);
				apply_dither(working_image, &quant_error, 5.f / 16.f, settings->width,
				             settings->height, x, y + 1, settings->dither_alpha);
				apply_dither(working_image, &quant_error, 1.f / 16.f, settings->width,
				             settings->height, x - 1, y + 1, settings->dither_alpha);
			}

			x += direction;
		}

		if (settings->serpentine)
			direction = -direction;
	}

	if (settings->verbose)
		puts("Finished quantization");

cleanup:
	if (rc == 0)
		puts("Premature ejection");

	return rc;
}

uint16_t* get_png_to_bin_cast(ProgramSettings* settings, RGBAF* working_image)
{
	uint16_t* bin;

	bin = malloc(settings->resolution * sizeof(*bin));

	if (!bin)
	{
		seprint("Unable to allocate memory for bin image");
		return NULL;
	}

	if (settings->verbose)
	{
		puts("Allocated memory for bin image");
		puts("Casting working image to bin");
	}

	for (int i = 0; i < settings->resolution; ++i)
	{
		RGBAF temp = find_closest_limited_color(&working_image[i], true);

		uint16_t r = ((uint16_t)temp.r) & ((1 << r_bit) - 1);
		uint16_t g = ((uint16_t)temp.g) & ((1 << g_bit) - 1);
		uint16_t b = ((uint16_t)temp.b) & ((1 << b_bit) - 1);
		uint16_t a = a_bit == 0 ? 0 : (((uint16_t)temp.a) & ((1 << a_bit) - 1));

		// MSB-aligned
		bin[i] = (uint16_t)((r << (16 - r_bit)) + (g << (16 - r_bit - g_bit))
		                    + (b << (16 - r_bit - g_bit - b_bit)) + a);
	}

	if (settings->verbose)
		puts("Finished casting working image to bin");

	return bin;
}
