#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "argparse.h"
#include "program_settings.h"
#include "rgbx.h"

const char* onoff_strings[2] = {"OFF", "ON"};

// clang-format off
static const char* const usage[] =
{
    "rgbx [options]",
    NULL
};
// clang-format on

void auto_renamer(char* input_file_path, char* extension, char* output_file_path)
{
	const char* last_slash = strrchr(input_file_path, '/');
	const char* filename = (last_slash) ? last_slash + 1 : input_file_path;

	// copy directory part
	size_t dir_len = (last_slash) ? (size_t)(last_slash - input_file_path + 1) : 0;

	if (dir_len > 0)
		strncpy(output_file_path, input_file_path, dir_len);

	output_file_path[dir_len] = '\0';

	// strip existing extension
	char base[256];
	strncpy(base, filename, sizeof(base));
	base[sizeof(base) - 1] = '\0';

	char* dot = strrchr(base, '.');

	if (dot)
		*dot = '\0'; // remove old extension

	// construct new filename
	snprintf(output_file_path + dir_len, 512 - dir_len, "%s_o.%s", base, extension);
}

void print_mode_help()
{
	const char* mode_help[] = {"This program has three modes:\n\n",
	                           "\tp2p - PNG to PNG (default)\n", "\tp2b - PNG to BIN\n",
	                           "\tb2p - BIN to PNG\n\n"};

	for (unsigned int i = 0; i < sizeof(mode_help) / sizeof(mode_help[0]); ++i)
	{
		printf("%s", mode_help[i]);
	}
}

int setup(int argc, const char** argv, ProgramSettings* settings)
{
	const char* mode_input = NULL;
	int pbd_input = 0;
	int dither_input = 0;
	int serpentine_input = 0;
	int dither_alpha_input = 0;
	int legal_input = 0;
	int verbose_input = 0;

	settings->input_file_path = NULL;
	settings->output_file_path = NULL;
	settings->auto_generated_ofname = false;

	struct argparse_option options[] = {
	    OPT_HELP(),
	    OPT_STRING('i', "input", &settings->input_file_path, "input file"),
	    OPT_STRING('o', "output", &settings->output_file_path, "output file"),
	    OPT_STRING('m', "mode", &mode_input, "mode, see --mode help"),
	    OPT_INTEGER('b', "bits", &pbd_input, "bits per component, e.g. -b 0565 for RGB565"),
	    OPT_BOOLEAN('d', "dither", &dither_input, "enable dithering"),
	    OPT_BOOLEAN('s', "serpentine", &serpentine_input, "enable serpentine dithering"),
	    OPT_BOOLEAN('a', "dither-alpha", &dither_alpha_input, "dither alpha"),
	    OPT_BOOLEAN('l', "legal", &legal_input, "show legal information and exit"),
	    OPT_BOOLEAN('v', "verbose", &verbose_input, "verbose output"),
	    OPT_END(),
	};

	struct argparse argparse_instance;
	argparse_init(&argparse_instance, options, usage, 0);
	argparse_instance.epilog =
	    "\nRegarding --bits:\n\nEach color component is stored in the ARGB format. So if you "
	    "wanted the\nRGB565 color space, you could either use 565 or 0565. This allows you to "
	    "skip\nthe alpha channel, which will default it to 0, or no transparency.\n";
	(void)argparse_parse(&argparse_instance, argc, argv);

	if (argc == 1)
	{
		argparse_usage(&argparse_instance);
		return 0;
	}

	settings->dither = dither_input == 1;
	settings->serpentine = serpentine_input == 1;
	settings->dither_alpha = dither_alpha_input == 1;
	settings->legal = legal_input == 1;
	settings->verbose = verbose_input == 1;

	// Skip to legal
	if (settings->legal)
		return 1;

	if (mode_input == NULL || strcmp(mode_input, "p2p") == 0 || strcmp(mode_input, "") == 0)
	{
		settings->mode = MODETYPE_PNG_TO_PNG;
	}
	else if (strcmp(mode_input, "p2b") == 0)
	{
		settings->mode = MODETYPE_PNG_TO_BIN;
	}
	else if (strcmp(mode_input, "b2p") == 0)
	{
		settings->mode = MODETYPE_BIN_TO_PNG;
	}
	else if (strcmp(mode_input, "help") == 0 || strcmp(mode_input, "h") == 0)
	{
		print_mode_help();
		return 0;
	}
	else
	{
		fprintf(stderr, "Unknown mode. See --mode help\n");
		return 0;
	}

	if (settings->input_file_path == NULL)
	{
		seprint("An input file must be provided");
		return 0;
	}

	if (settings->output_file_path == NULL)
	{
		settings->auto_generated_ofname = true;
		settings->output_file_path = malloc(256);

		if (!settings->output_file_path)
		{
			seprint("Error allocating memory for auto generated file output path");
			return 0;
		}

		if (settings->mode == MODETYPE_PNG_TO_BIN)
			auto_renamer(settings->input_file_path, "bin", settings->output_file_path);
		else
			auto_renamer(settings->input_file_path, "png", settings->output_file_path);
	}

	// Skip bit depth if we're reading from a bin file
	if (settings->mode == MODETYPE_BIN_TO_PNG)
		return 1;

	settings->packed_bit_depth = (int16_t)pbd_input;

	if (settings->packed_bit_depth == 0)
	{
		seprint("--bits is required");
		return 0;
	}

	if (!handle_bit_depths(settings))
		return 0;

	return 1;
}
