#ifndef RGBX_H
#define RGBX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "legal.h"
#include "program_settings.h"
#include "rgb_bits.h"
#include "rgbaf.h"

extern const char* onoff_strings[2];

int setup(int argc, char** argv, ProgramSettings* settings);
int handle_bit_depths(ProgramSettings* settings);
uint16_t* get_png_to_bin(ProgramSettings* settings);
uint16_t* get_file_to_bin(ProgramSettings* settings);
int get_bin_to_file(ProgramSettings* settings, uint16_t* bin);
int get_png_to_file(ProgramSettings* settings, uint16_t* bin);

#ifdef __cplusplus
}
#endif

#endif
