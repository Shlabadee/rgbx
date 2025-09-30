#ifndef RGBA_BITS_H
#define RGBA_BITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "program_settings.h"

extern int r_bit, g_bit, b_bit, a_bit;
extern int r_max, g_max, b_max, a_max;
extern float r_maxf, g_maxf, b_maxf, a_maxf;

void unpack_bit_depths(int16_t pbd, int* r, int* g, int* b, int* a);
void repack_bit_depths(int16_t* pbd, int r, int g, int b, int a);
int handle_bit_depths(ProgramSettings* settings);

#ifdef __cplusplus
}
#endif

#endif
