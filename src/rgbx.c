#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "argo.h"
#include "program_settings.h"
#include "rgbx.h"

#define OPTION_HELP 0
#define OPTION_VERSION 1
#define OPTION_INPUT 2
#define OPTION_OUTPUT 3
#define OPTION_MODE 4
#define OPTION_BITS 5
#define OPTION_DITHER 6
#define OPTION_SERPENTINE 7
#define OPTION_ALPHA 8
#define OPTION_LEGAL 9
#define OPTION_VERBOSE 10

const char* onoff_strings[2] = {"OFF", "ON"};

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

void print_mode_help(void)
{
	const char* mode_help[] = {"This program has three modes:\n\n",
	                           "\tp2p - PNG to PNG (default)\n", "\tp2b - PNG to BIN\n",
	                           "\tb2p - BIN to PNG\n\n"};

	for (unsigned int i = 0; i < sizeof(mode_help) / sizeof(mode_help[0]); ++i)
	{
		printf("%s", mode_help[i]);
	}
}

int setup(int argc, char** argv, ProgramSettings* settings)
{
	char* mode_input = NULL;

	settings->input_file_path = NULL;
	settings->output_file_path = NULL;
	settings->auto_generated_ofname = false;

	ArgoOption options[] = {
	    Argo_Set('h', "help", ArgoOptionType_Boolean, "show this help message and exit"),
	    Argo_Set(Argo_NoShortFlag, "version", ArgoOptionType_Boolean,
	             "show the program version and exit"),
	    Argo_Set('i', "input", ArgoOptionType_String, "input file"),
	    Argo_Set('o', "output", ArgoOptionType_String, "output file"),
	    Argo_Set('m', "mode", ArgoOptionType_String, "mode, see --mode help"),
	    Argo_Set('b', "bits", ArgoOptionType_Integer,
	             "bits per component, e.g. -b 0565 for RGB565"),
	    Argo_Set('d', "dither", ArgoOptionType_Boolean, "enable dithering"),
	    Argo_Set('s', "serpentine", ArgoOptionType_Boolean, "enable serpentine dithering"),
	    Argo_Set('a', "alpha-dithering", ArgoOptionType_Boolean, "dither alpha"),
	    Argo_Set('l', "legal", ArgoOptionType_Boolean, "show legal information and exit"),
	    Argo_Set('v', "verbose", ArgoOptionType_Boolean, "verbose output"),
	};

	ArgoInstance instance;
	ArgoReturnType rt = Argo_Tokenize(&instance, options, sizeof(options) / sizeof(options[0]),
	                                  argc, argv, true);

	if (rt == ArgoReturnType_Failure)
	{
		Argo_PrintError();
		return 0;
	}

	if (argc == 1 || options[OPTION_HELP].found)
	{
		puts("");
		Argo_Help(&instance);
		printf("Regarding --bits:\n\nEach color component is stored in the ARGB format. So "
		       "if you wanted the\nRGB565 color space, you could either use 565 or 0565. This "
		       "allows you to skip\nthe alpha channel, which will default it to 0, or no "
		       "transparency.\n\n");
		return 0;
	}

	settings->dither = options[OPTION_DITHER].found;
	settings->serpentine = options[OPTION_SERPENTINE].found;
	settings->dither_alpha = options[OPTION_ALPHA].found;
	settings->legal = options[OPTION_LEGAL].found;
	settings->show_version = options[OPTION_VERSION].found;
	settings->verbose = options[OPTION_VERBOSE].found;

	// Just show the current version
	if (settings->show_version)
		return 1;

	// Skip to legal
	if (settings->legal)
		return 1;

	mode_input = options[OPTION_MODE].value;

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

	settings->input_file_path = options[OPTION_INPUT].value;

	if (settings->input_file_path == NULL)
	{
		seprint("An input file must be provided");
		return 0;
	}

	settings->output_file_path = options[OPTION_OUTPUT].value;

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

	if (options[OPTION_BITS].value == NULL)
	{
		seprint("--bits is required");
		return 0;
	}

	settings->packed_bit_depth = (int16_t)strtol(options[OPTION_BITS].value, NULL, 10);

	if (!handle_bit_depths(settings))
		return 0;

	return 1;
}
