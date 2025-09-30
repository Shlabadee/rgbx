#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "magic_number.h"
#include "program_settings.h"
#include "rgb_bits.h"

int get_bin_to_file(ProgramSettings* settings, uint16_t* bin)
{
	int rc = 1;
	FILE* output_file = fopen(settings->output_file_path, "wb");

	if (!output_file)
	{
		seprint("Error opening output file");
		return 0;
	}

	if (settings->verbose)
		puts("Exporting bin to file");

	repack_bit_depths(&settings->packed_bit_depth, r_bit, g_bit, b_bit, a_bit);

	fwrite(&RGBX_MAGIC_NUMBER, 4, 1, output_file);

	fwrite(&settings->width, sizeof(settings->width), 1, output_file);
	fwrite(&settings->height, sizeof(settings->height), 1, output_file);
	fwrite(&settings->packed_bit_depth, sizeof(settings->packed_bit_depth), 1, output_file);
	fwrite(bin, sizeof(*bin), settings->resolution, output_file);

	return rc;
}

uint16_t* get_file_to_bin(ProgramSettings* settings)
{
	uint16_t* bin = NULL;
	bool error_reading = false;

	FILE* input_file = fopen(settings->input_file_path, "rb");

	if (!input_file)
	{
		seprint("Error opening input file for bin");
		return NULL;
	}

	uint32_t magic_number;

	if (fread(&magic_number, 4, 1, input_file) != 1)
	{
		seprint("Error reading magic number for bin input file");
		goto cleanup;
	}

	if (magic_number != RGBX_MAGIC_NUMBER)
	{
		seprint("Improper bin input file format");
		goto cleanup;
	}

	if (fread(&settings->width, 4, 1, input_file) != 1)
	{
		seprint("Error reading width for bin input file");
		goto cleanup;
	}

	if (fread(&settings->height, 4, 1, input_file) != 1)
	{
		seprint("Error reading height for bin input file");
		goto cleanup;
	}

	if (fread(&settings->packed_bit_depth, 2, 1, input_file) != 1)
	{
		seprint("Error reading packed bit depth for bin input file");
		goto cleanup;
	}

	if (!handle_bit_depths(settings))
		goto cleanup;

	if (settings->verbose)
		puts("Calculating resolution");

	settings->resolution = settings->width * settings->height;

	bin = malloc(settings->resolution * sizeof(*bin));

	if (!bin)
	{
		seprint("Error allocating memory for input bin");
		goto cleanup;
	}

	if (fread(bin, 2, settings->resolution, input_file) != (unsigned)settings->resolution)
	{
		seprint("Error reading all image data from bin input file");
		error_reading = true;
		goto cleanup;
	}

cleanup:
	if (error_reading)
	{
		free(bin);
		bin = NULL;
	}

	if (input_file)
		fclose(input_file);

	return bin;
}
