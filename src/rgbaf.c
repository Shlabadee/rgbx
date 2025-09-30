#include <stdbool.h>

#include <string.h>

#include "rgb_bits.h"
#include "rgbaf.h"

static inline float l_roundf(float n)
{
	return (float)((int)(n + 0.5f));
}

static inline float l_clampf(float v, float lo, float hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

RGBAF* RGBAF_get(RGBAF* input_image, int width, int height, int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return NULL;

	return &input_image[(y * width) + x];
}

RGBAF RGBAF_copy(RGBAF* src)
{
	RGBAF copy;
	memcpy(&copy, src, sizeof(RGBAF));
	return copy;
}

RGBAF find_closest_limited_color(RGBAF* oldpixel, bool round)
{
	RGBAF newpixel;

	newpixel.r = l_roundf((oldpixel->r / 255.f) * r_maxf);
	newpixel.g = l_roundf((oldpixel->g / 255.f) * g_maxf);
	newpixel.b = l_roundf((oldpixel->b / 255.f) * b_maxf);
	newpixel.a = l_roundf((oldpixel->a / 255.f) * a_maxf);

	if (round)
	{
		newpixel.r = l_roundf(newpixel.r);
		newpixel.g = l_roundf(newpixel.g);
		newpixel.b = l_roundf(newpixel.b);
		newpixel.a = l_roundf(newpixel.a);
	}

	return newpixel;
}

RGBAF promote_pixel(RGBAF* oldpixel, bool round)
{
	RGBAF newpixel;

	newpixel.r = (oldpixel->r / r_maxf) * 255.f;
	newpixel.g = (oldpixel->g / g_maxf) * 255.f;
	newpixel.b = (oldpixel->b / b_maxf) * 255.f;
	newpixel.a = (oldpixel->a / a_maxf) * 255.f;

	if (round)
	{
		newpixel.r = l_roundf(newpixel.r);
		newpixel.g = l_roundf(newpixel.g);
		newpixel.b = l_roundf(newpixel.b);
		newpixel.a = l_roundf(newpixel.a);
	}

	return newpixel;
}

RGBAF RGBAF_add(RGBAF* a, RGBAF* b)
{
	RGBAF c;

	c.r = a->r + b->r;
	c.g = a->g + b->g;
	c.b = a->b + b->b;
	c.a = a->a + b->a;

	return c;
}

RGBAF RGBAF_subtract(RGBAF* a, RGBAF* b)
{
	RGBAF c;

	c.r = a->r - b->r;
	c.g = a->g - b->g;
	c.b = a->b - b->b;
	c.a = a->a - b->a;

	return c;
}

RGBAF RGBAF_multiply(RGBAF* a, float b)
{
	RGBAF c;

	c.r = a->r * b;
	c.g = a->g * b;
	c.b = a->b * b;
	c.a = a->a * b;

	return c;
}

bool apply_dither(RGBAF* working_image, RGBAF* quant_error, float weight, int width, int height,
                  int x, int y, bool dither_alpha)
{
	RGBAF* current_pixel = RGBAF_get(working_image, width, height, x, y);

	if (!current_pixel)
		return false;

	RGBAF temp;
	temp = RGBAF_multiply(quant_error, weight);
	temp = RGBAF_add(&temp, current_pixel);
	temp.r = l_clampf(temp.r, 0.f, 255.f);
	temp.g = l_clampf(temp.g, 0.f, 255.f);
	temp.b = l_clampf(temp.b, 0.f, 255.f);

	if (dither_alpha)
		temp.a = l_clampf(temp.a, 0.f, 255.f);

	*current_pixel = temp;

	return true;
}
