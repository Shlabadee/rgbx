#include <stdint.h>
#include <stdio.h>

#include "program_settings.h"
#include "rgb_bits.h"

int r_bit, g_bit, b_bit, a_bit;
int r_max, g_max, b_max, a_max;
float r_maxf, g_maxf, b_maxf, a_maxf;

void unpack_bit_depths(int16_t pbd, int* r, int* g, int* b, int* a)
{
	*b = pbd % 10;

	pbd /= 10;
	*g = pbd % 10;

	pbd /= 10;
	*r = pbd % 10;

	pbd /= 10;
	*a = pbd % 10;
}

void repack_bit_depths(int16_t* pbd, int r, int g, int b, int a)
{
	*pbd = a;

	*pbd *= 10;
	*pbd += r;

	*pbd *= 10;
	*pbd += g;

	*pbd *= 10;
	*pbd += b;
}

int handle_bit_depths(ProgramSettings* settings)
{
	int total_bit_depth;

	unpack_bit_depths(settings->packed_bit_depth, &r_bit, &g_bit, &b_bit, &a_bit);

	total_bit_depth = r_bit + g_bit + b_bit + a_bit;

	r_max = (1 << r_bit) - 1;
	g_max = (1 << g_bit) - 1;
	b_max = (1 << b_bit) - 1;
	a_max = (1 << a_bit) - 1;

	r_maxf = (float)r_max;
	g_maxf = (float)g_max;
	b_maxf = (float)b_max;
	a_maxf = (float)a_max;

	if (settings->verbose)
	{
		printf("Packed bit depth: %i\n", settings->packed_bit_depth);
		printf("r_bit: %i\n", r_max);
		printf("g_bit: %i\n", g_max);
		printf("b_bit: %i\n", b_max);
		printf("a_bit: %i\n", a_max);
		printf("Total bit depth: %i\n", total_bit_depth);
	}

	if (total_bit_depth > 16)
	{
		fprintf(stderr, "Bit depth greater than 16. Received: %i\n", total_bit_depth);
		return 0;
	}

	return 1;
}