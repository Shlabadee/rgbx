#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "program_settings.h"
#include "rgbaf.h"

#include "encoder_helper.h"

uint16_t* get_png_to_bin(ProgramSettings* settings)
{
	uint16_t* bin = NULL;
	RGBAF* working_image = get_png_to_bin_setup(settings);

	if (!working_image)
		goto cleanup;

	if (!get_png_to_bin_quantize(settings, working_image))
		goto cleanup;

	bin = get_png_to_bin_cast(settings, working_image);
	if (!bin)
		goto cleanup;

cleanup:
	if (working_image)
	{
		if (settings->verbose)
			puts("Freeing working image");

		free(working_image);
	}

	return bin;
}
