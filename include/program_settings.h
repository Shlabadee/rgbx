#ifndef PROGRAM_SETTINGS_H
#define PROGRAM_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum ModeType
{
	MODETYPE_PNG_TO_PNG,
	MODETYPE_PNG_TO_BIN,
	MODETYPE_BIN_TO_PNG
} ModeType;

typedef struct ProgramSettings
{
	char* input_file_path;
	char* output_file_path;
	ModeType mode;
	int16_t packed_bit_depth;
	int width, height;
	int resolution;
	bool dither;
	bool serpentine;
	bool dither_alpha;
	bool legal;
	bool verbose;
	bool auto_generated_ofname;
} ProgramSettings;

static inline void seprint(const char* msg)
{
	fprintf(stderr, "%s\n", msg);
}

#ifdef __cplusplus
}
#endif

#endif
