#ifndef RGBAF_H
#define RGBAF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct RGBAF
{
	float r, g, b, a;
} RGBAF;

RGBAF* RGBAF_get(RGBAF* input_image, int width, int height, int x, int y);
RGBAF RGBAF_copy(RGBAF* src);
RGBAF find_closest_limited_color(RGBAF* oldpixel, bool round);
RGBAF promote_pixel(RGBAF* oldpixel, bool round);
RGBAF RGBAF_add(RGBAF* a, RGBAF* b);
RGBAF RGBAF_subtract(RGBAF* a, RGBAF* b);
RGBAF RGBAF_multiply(RGBAF* a, float b);
bool apply_dither(RGBAF* working_image, RGBAF* quant_error, float weight, int width, int height,
                  int x, int y, bool dither_alpha);

#ifdef __cplusplus
}
#endif

#endif
