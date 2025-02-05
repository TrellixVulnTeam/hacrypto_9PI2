#include "vec128.h"
//#include <immintrin.h>

extern void vec128_mul_asm(vec128 *, vec128 *, vec128 *);

void vec128_mul(vec128 *h, vec128 *f, vec128 *g)
{
	vec128_mul_asm(h, f, g);
}

void vec128_sq(vec128 * out, vec128 * in)
{
	int i;
	vec128 result[GFBITS], t;

	t = vec128_xor(in[11], in[12]);

	result[0] = vec128_xor(in[0], in[11]);
	result[1] = vec128_xor(in[7], t);
	result[2] = vec128_xor(in[1], in[7]);
	result[3] = vec128_xor(in[8], t);
	result[4] = vec128_xor(in[2], in[7]);
	result[4] = vec128_xor(result[4], in[8]);
	result[4] = vec128_xor(result[4], t);
	result[5] = vec128_xor(in[7], in[9]);
	result[6] = vec128_xor(in[3], in[8]);
	result[6] = vec128_xor(result[6], in[9]);
	result[6] = vec128_xor(result[6], in[12]);
	result[7] = vec128_xor(in[8], in[10]);
	result[8] = vec128_xor(in[4], in[9]);
	result[8] = vec128_xor(result[8], in[10]);
	result[9] = vec128_xor(in[9], in[11]);
	result[10] = vec128_xor(in[5], in[10]);
	result[10] = vec128_xor(result[10], in[11]);
	result[11] = vec128_xor(in[10], in[12]);
	result[12] = vec128_xor(in[6], t);

	for (i = 0; i < GFBITS; i++)
		out[i] = result[i];
}

void vec128_inv(vec128 * out, vec128 * in)
{
	vec128 tmp_11[ GFBITS ];
	vec128 tmp_1111[ GFBITS ];

	vec128_copy(out, in);

	vec128_sq(out, out);
	vec128_mul(tmp_11, out, in); // ^11

	vec128_sq(out, tmp_11);
	vec128_sq(out, out);
	vec128_mul(tmp_1111, out, tmp_11); // ^1111

	vec128_sq(out, tmp_1111);
	vec128_sq(out, out);
	vec128_sq(out, out);
	vec128_sq(out, out);
	vec128_mul(out, out, tmp_1111); // ^11111111

	vec128_sq(out, out);
	vec128_sq(out, out);
	vec128_sq(out, out);
	vec128_sq(out, out);
	vec128_mul(out, out, tmp_1111); // ^111111111111

	vec128_sq(out, out); // ^1111111111110
}

