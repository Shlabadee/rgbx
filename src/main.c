#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "rgbx.h"

int main(int argc, const char** argv)
{
	printf("rgbx v1.0.0 - Shlabadee\n");
	int rc = 0;
	uint16_t* bin = NULL;
	ProgramSettings settings;

	if (!setup(argc, argv, &settings))
	{
		rc = 1;
		goto cleanup;
	}

	if (settings.verbose)
	{
		printf("Dithering is %s\n", onoff_strings[settings.dither]);
		printf("Serpentine dithering is %s\n", onoff_strings[settings.serpentine]);
		printf("Alpha dithering is %s\n", onoff_strings[settings.dither_alpha]);
	}

	if (settings.legal)
	{
		print_legal();
		return 0;
	}

	// First, let's get the bin file by any means
	if (settings.mode != MODETYPE_BIN_TO_PNG)
	{
		bin = get_png_to_bin(&settings);

		if (!bin)
		{
			seprint("Error converting PNG to BIN");
			rc = 1;
			goto cleanup;
		}

		if (settings.verbose)
			puts("Successfully read PNG file to bin format");
	}
	else
	{
		bin = get_file_to_bin(&settings);

		if (!bin)
		{
			seprint("Error getting data from bin");
			rc = 1;
			goto cleanup;
		}

		if (settings.verbose)
			puts("Successfully read bin file");
	}

	// Do we need to export the raw bin to a file?
	if (settings.mode == MODETYPE_PNG_TO_BIN)
	{
		if (!get_bin_to_file(&settings, bin))
		{
			rc = 1;
			goto cleanup;
		}
	}
	else // If not, let's reconvert back to a PNG file.
	{
		if (!get_png_to_file(&settings, bin))
		{
			rc = 1;
			goto cleanup;
		}
	}

cleanup:
	if (bin)
		free(bin);

	if (settings.auto_generated_ofname)
		free(settings.output_file_path);

	return rc;
}
