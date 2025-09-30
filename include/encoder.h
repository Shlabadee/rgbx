#ifndef ENCODER_H
#define ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "program_settings.h"
#include "rgbaf.h"

RGBAF* get_png_to_bin_setup(ProgramSettings* settings);
int get_png_to_bin_quantize(ProgramSettings* settings, RGBAF* working_image);
uint16_t* get_png_to_bin_cast(ProgramSettings* settings, RGBAF* working_image);
uint16_t* get_png_to_bin(ProgramSettings* settings);

#ifdef __cplusplus
}
#endif

#endif
