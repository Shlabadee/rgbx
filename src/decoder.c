#include <stdint.h>
#include <stdlib.h>

#include "ezpng.h"
#include "program_settings.h"
#include "rgb_bits.h"

int get_png_to_file(ProgramSettings* settings, uint16_t* bin)
{
	int rc = 1;

	ezpng_rgba* output_image = malloc(settings->resolution * sizeof(*output_image));

	if (!output_image)
	{
		seprint("Error allocating memory for PNG output image");
		return 0;
	}

	for (int i = 0; i < settings->resolution; ++i)
	{
		uint16_t packed = bin[i];

		int gap = 16 - (r_bit + g_bit + b_bit + a_bit);

		uint16_t r = (packed >> (g_bit + b_bit + a_bit + gap)) & ((1u << r_bit) - 1);
		uint16_t g = (packed >> (b_bit + a_bit + gap)) & ((1u << g_bit) - 1);
		uint16_t b = (packed >> (a_bit + gap)) & ((1u << b_bit) - 1);
		uint16_t a = packed & ((1u << a_bit) - 1);

		output_image[i].r =
		    r_bit ? (uint16_t)(((r / ((float)((1 << r_bit) - 1))) * 255.f) + 0.5f) : 0;
		output_image[i].g =
		    g_bit ? (uint16_t)(((g / ((float)((1 << g_bit) - 1))) * 255.f) + 0.5f) : 0;
		output_image[i].b =
		    b_bit ? (uint16_t)(((b / ((float)((1 << b_bit) - 1))) * 255.f) + 0.5f) : 0;
		output_image[i].a =
		    a_bit ? (uint16_t)(((a / ((float)((1 << a_bit) - 1))) * 255.f) + 0.5f) : 255;
	}

	ezpng_write_rgba(settings->output_file_path, output_image, settings->width,
	                 settings->height);

	if (output_image)
		free(output_image);

	return rc;
}