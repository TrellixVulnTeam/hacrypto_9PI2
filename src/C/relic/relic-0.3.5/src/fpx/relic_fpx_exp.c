/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2013 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Implementation of exponentiation in extensions defined over prime fields.
 *
 * @version $Id: relic_fpx_exp.c 1551 2013-08-29 23:04:08Z dfaranha $
 * @ingroup fpx
 */

#include "relic_core.h"
#include "relic_pp_low.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void fp2_exp(fp2_t c, fp2_t a, bn_t b) {
	fp2_t t;

	fp2_null(t);

	TRY {
		fp2_new(t);

		fp2_copy(t, a);

		for (int i = bn_bits(b) - 2; i >= 0; i--) {
			fp2_sqr(t, t);
			if (bn_test_bit(b, i)) {
				fp2_mul(t, t, a);
			}
		}
		fp2_copy(c, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp2_free(t);
	}
}

void fp2_conv_uni(fp2_t c, fp2_t a) {
	fp2_t t;

	fp2_null(t);

	TRY {
		fp2_new(t);

		/* t = a^{-1}. */
		fp2_inv(t, a);
		/* c = a^p. */
		fp2_inv_uni(c, a);
		/* c = a^(p - 1). */
		fp2_mul(c, c, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp2_free(t);
	}
}


void fp2_exp_uni(fp2_t c, fp2_t a, bn_t b) {
	fp2_t r, s, t[1 << (FP_WIDTH - 2)];
	int i, l;
	signed char naf[FP_BITS + 1], *k, n;

	fp2_null(r);
	fp2_null(s);

	TRY {
		fp2_new(r);
		fp2_new(s);
		for (i = 0; i < (1 << (FP_WIDTH - 2)); i ++) {
			fp2_null(t[i]);
			fp2_new(t[i]);
		}

#if FP_WIDTH > 2
		fp2_sqr(t[0], a);
		fp2_mul(t[1], t[0], a);
		for (int i = 2; i < (1 << (FP_WIDTH - 2)); i++) {
			fp2_mul(t[i], t[i - 1], t[0]);
		}
#endif
		fp2_copy(t[0], a);

		l = FP_BITS + 1;
		fp2_zero(r);
		fp_set_dig(r[0], 1);
		bn_rec_naf(naf, &l, b, FP_WIDTH);

		k = naf + l - 1;

		for (i = l - 1; i >= 0; i--, k--) {
			fp2_sqr(r, r);

			n = *k;
			if (n > 0) {
				fp2_mul(r, r, t[n / 2]);
			}
			if (n < 0) {
				fp2_inv_uni(s, t[-n / 2]);
				fp2_mul(r, r, s);
			}
		}

		fp2_copy(c, r);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp2_free(r);
		fp2_free(s);
		for (i = 0; i < (1 << (FP_WIDTH - 2)); i++) {
			fp2_free(t[i]);
		}
	}
}

void fp3_exp(fp3_t c, fp3_t a, bn_t b) {
	fp3_t t;

	fp3_null(t);

	TRY {
		fp3_new(t);

		fp3_copy(t, a);

		for (int i = bn_bits(b) - 2; i >= 0; i--) {
			fp3_sqr(t, t);
			if (bn_test_bit(b, i)) {
				fp3_mul(t, t, a);
			}
		}
		fp3_copy(c, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp3_free(t);
	}
}

void fp6_exp(fp6_t c, fp6_t a, bn_t b) {
	fp6_t t;

	fp6_null(t);

	TRY {
		fp6_new(t);

		fp6_copy(t, a);

		for (int i = bn_bits(b) - 2; i >= 0; i--) {
			fp6_sqr(t, t);
			if (bn_test_bit(b, i)) {
				fp6_mul(t, t, a);
			}
		}
		fp6_copy(c, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp6_free(t);
	}
}

void fp12_exp(fp12_t c, fp12_t a, bn_t b) {
	fp12_t t;

	fp12_null(t);

	TRY {
		fp12_new(t);

		if (fp12_test_cyc(a)) {
			fp12_exp_cyc(c, a, b);
		} else {
			fp12_copy(t, a);

			for (int i = bn_bits(b) - 2; i >= 0; i--) {
				fp12_sqr(t, t);
				if (bn_test_bit(b, i)) {
					fp12_mul(t, t, a);
				}
			}
			fp12_copy(c, t);
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp12_free(t);
	}
}

void fp12_exp_cyc(fp12_t c, fp12_t a, bn_t b) {
	fp12_t t;
	int i, j, k, w = bn_ham(b);

	fp12_null(t);

	if (w > (bn_bits(b) >> 3)) {
		TRY {
			fp12_new(t);

			fp12_copy(t, a);

			for (i = bn_bits(b) - 2; i >= 0; i--) {
				fp12_sqr_cyc(t, t);
				if (bn_test_bit(b, i)) {
					fp12_mul(t, t, a);
				}
			}
			fp12_copy(c, t);
		}
		CATCH_ANY {
			THROW(ERR_CAUGHT);
		}
		FINALLY {
			fp12_free(t);
		}
	} else {
		fp12_t u[w];

		TRY {
			for (i = 0; i < w; i++) {
				fp12_null(u[i]);
				fp12_new(u[i]);
			}
			fp12_new(t);

			j = 0;
			fp12_copy(t, a);
			for (i = 1; i < bn_bits(b); i++) {
				fp12_sqr_pck(t, t);
				if (bn_test_bit(b, i)) {
					fp12_copy(u[j++], t);
				}
			}

			if (!bn_is_even(b)) {
				j = 0;
				k = w - 1;
			} else {
				j = 1;
				k = w;
			}

			fp12_back_cyc_sim(u, u, k);

			if (!bn_is_even(b)) {
				fp12_copy(c, a);
			} else {
				fp12_copy(c, u[0]);
			}

			for (i = j; i < k; i++) {
				fp12_mul(c, c, u[i]);
			}
		}
		CATCH_ANY {
			THROW(ERR_CAUGHT);
		}
		FINALLY {
			for (i = 0; i < w; i++) {
				fp12_free(u[i]);
			}
			fp12_free(t);
		}
	}
}

void fp12_exp_cyc_sps(fp12_t c, fp12_t a, int *b, int len) {
	int i, j, k, w = len;
	fp12_t t, u[w];

	fp12_null(t);

	TRY {
		for (i = 0; i < w; i++) {
			fp12_null(u[i]);
			fp12_new(u[i]);
		}
		fp12_new(t);

		fp12_copy(t, a);
		if (b[0] == 0) {
			for (j = 0, i = 1; i < len; i++) {
				k = (b[i] < 0 ? -b[i] : b[i]);
				for (; j < k; j++) {
					fp12_sqr_pck(t, t);
				}
				if (b[i] < 0) {
					fp12_inv_uni(u[i - 1], t);
				} else {
					fp12_copy(u[i - 1], t);
				}
			}

			fp12_back_cyc_sim(u, u, w - 1);

			fp12_copy(c, a);
			for (i = 0; i < w - 1; i++) {
				fp12_mul(c, c, u[i]);
			}
		} else {
			for (j = 0, i = 0; i < len; i++) {
				k = (b[i] < 0 ? -b[i] : b[i]);
				for (; j < k; j++) {
					fp12_sqr_pck(t, t);
				}
				if (b[i] < 0) {
					fp12_inv_uni(u[i], t);
				} else {
					fp12_copy(u[i], t);
				}
			}

			fp12_back_cyc_sim(u, u, w);

			fp12_copy(c, u[0]);
			for (i = 1; i < w; i++) {
				fp12_mul(c, c, u[i]);
			}
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		for (i = 0; i < w; i++) {
			fp12_free(u[i]);
		}
		fp12_free(t);
	}
}

void fp12_conv_uni(fp12_t c, fp12_t a) {
	fp12_t t;

	fp12_null(t);

	TRY {
		fp12_new(t);

		/* t = a^{-1}. */
		fp12_inv(t, a);
		/* c = a^(p^6). */
		fp12_inv_uni(c, a);
		/* c = a^(p^6 - 1). */
		fp12_mul(c, c, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp12_free(t);
	}
}

void fp12_conv_cyc(fp12_t c, fp12_t a) {
	fp12_t t;

	fp12_null(t);

	TRY {
		fp12_new(t);

		/* First, compute c = a^(p^6 - 1). */
		/* t = a^{-1}. */
		fp12_inv(t, a);
		/* c = a^(p^6). */
		fp12_inv_uni(c, a);
		/* c = a^(p^6 - 1). */
		fp12_mul(c, c, t);

		/* Second, compute c^(p^2 + 1). */
		/* t = c^(p^2). */
		fp12_frb(t, c, 2);
		/* c = c^(p^2 + 1). */
		fp12_mul(c, c, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp12_free(t);
	}
}

int fp12_test_cyc(fp12_t a) {
	fp12_t t;
	int result = 0;

	fp12_null(t);

	TRY {
		fp12_new(t);

		fp12_back_cyc(t, a);
		result = ((fp12_cmp(t, a) == CMP_EQ) ? 1 : 0);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp12_free(t);
	}

	return result;
}

void fp12_back_cyc(fp12_t c, fp12_t a) {
	fp2_t t0, t1, t2;

	fp2_null(t0);
	fp2_null(t1);
	fp2_null(t2);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		fp2_new(t2);

		/* t0 = g4^2. */
		fp2_sqr(t0, a[0][1]);
		/* t1 = 3 * g4^2 - 2 * g3. */
		fp2_sub(t1, t0, a[0][2]);
		fp2_dbl(t1, t1);
		fp2_add(t1, t1, t0);
		/* t0 = E * g5^2 + t1. */
		fp2_sqr(t2, a[1][2]);
		fp2_mul_nor(t0, t2);
		fp2_add(t0, t0, t1);
		/* t1 = 1/(4 * g2). */
		fp2_dbl(t1, a[1][0]);
		fp2_dbl(t1, t1);
		fp2_inv(t1, t1);
		/* c_1 = g1. */
		fp2_mul(c[1][1], t0, t1);

		/* t1 = g3 * g4. */
		fp2_mul(t1, a[0][2], a[0][1]);
		/* t2 = 2 * g1^2 - 3 * g3 * g4. */
		fp2_sqr(t2, c[1][1]);
		fp2_sub(t2, t2, t1);
		fp2_dbl(t2, t2);
		fp2_sub(t2, t2, t1);
		/* t1 = g2 * g5. */
		fp2_mul(t1, a[1][0], a[1][2]);
		/* c_0 = E * (2 * g1^2 + g2 * g5 - 3 * g3 * g4) + 1. */
		fp2_add(t2, t2, t1);
		fp2_mul_nor(c[0][0], t2);
		fp_add_dig(c[0][0][0], c[0][0][0], 1);

		fp2_copy(c[0][1], a[0][1]);
		fp2_copy(c[0][2], a[0][2]);
		fp2_copy(c[1][0], a[1][0]);
		fp2_copy(c[1][2], a[1][2]);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp2_free(t0);
		fp2_free(t1);
		fp2_free(t2);
	}
}

void fp12_back_cyc_sim(fp12_t c[], fp12_t a[], int n) {
	fp2_t t0[n], t1[n], t2[n];

	for (int i = 0; i < n; i++) {
		fp2_null(t0[i]);
		fp2_null(t1[i]);
		fp2_null(t2[i]);
	}

	TRY {
		for (int i = 0; i < n; i++) {
			fp2_new(t0[i]);
			fp2_new(t1[i]);
			fp2_new(t2[i]);
		}

		for (int i = 0; i < n; i++) {
			/* t0 = g4^2. */
			fp2_sqr(t0[i], a[i][0][1]);
			/* t1 = 3 * g4^2 - 2 * g3. */
			fp2_sub(t1[i], t0[i], a[i][0][2]);
			fp2_dbl(t1[i], t1[i]);
			fp2_add(t1[i], t1[i], t0[i]);
			/* t0 = E * g5^2 + t1. */
			fp2_sqr(t2[i], a[i][1][2]);
			fp2_mul_nor(t0[i], t2[i]);
			fp2_add(t0[i], t0[i], t1[i]);
			/* t1 = (4 * g2). */
			fp2_dbl(t1[i], a[i][1][0]);
			fp2_dbl(t1[i], t1[i]);
		}

		/* t1 = 1 / t1. */
		fp2_inv_sim(t1, t1, n);

		for (int i = 0; i < n; i++) {
			/* t0 = g1. */
			fp2_mul(c[i][1][1], t0[i], t1[i]);

			/* t1 = g3 * g4. */
			fp2_mul(t1[i], a[i][0][2], a[i][0][1]);
			/* t2 = 2 * g1^2 - 3 * g3 * g4. */
			fp2_sqr(t2[i], c[i][1][1]);
			fp2_sub(t2[i], t2[i], t1[i]);
			fp2_dbl(t2[i], t2[i]);
			fp2_sub(t2[i], t2[i], t1[i]);
			/* t1 = g2 * g5. */
			fp2_mul(t1[i], a[i][1][0], a[i][1][2]);
			/* t2 = E * (2 * g1^2 + g2 * g5 - 3 * g3 * g4) + 1. */
			fp2_add(t2[i], t2[i], t1[i]);
			fp2_mul_nor(c[i][0][0], t2[i]);
			fp_add_dig(c[i][0][0][0], c[i][0][0][0], 1);

			fp2_copy(c[i][0][1], a[i][0][1]);
			fp2_copy(c[i][0][2], a[i][0][2]);
			fp2_copy(c[i][1][0], a[i][1][0]);
			fp2_copy(c[i][1][2], a[i][1][2]);
		}
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		for (int i = 0; i < n; i++) {
			fp2_free(t0[i]);
			fp2_free(t1[i]);
			fp2_free(t2[i]);
		}
	}
}

void fp18_exp(fp18_t c, fp18_t a, bn_t b) {
	fp18_t t;

	fp18_null(t);

	TRY {
		fp18_new(t);

		if (fp18_test_cyc(a)) {
			fp18_exp_cyc(c, a, b);
		} else {
			fp18_copy(t, a);

			for (int i = bn_bits(b) - 2; i >= 0; i--) {
				fp18_sqr(t, t);
				if (bn_test_bit(b, i)) {
					fp18_mul(t, t, a);
				}
			}
			fp18_copy(c, t);
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp18_free(t);
	}
}

void fp18_exp_cyc(fp18_t c, fp18_t a, bn_t b) {
	fp18_t t;
	int i, j, k, w = bn_ham(b);

	fp18_null(t);

	if (w > (bn_bits(b) >> 3)) {
		TRY {
			fp18_new(t);

			fp18_copy(t, a);

			for (i = bn_bits(b) - 2; i >= 0; i--) {
				fp18_sqr_cyc(t, t);
				if (bn_test_bit(b, i)) {
					fp18_mul(t, t, a);
				}
			}
			fp18_copy(c, t);
		}
		CATCH_ANY {
			THROW(ERR_CAUGHT);
		}
		FINALLY {
			fp18_free(t);
		}
	} else {
		fp18_t u[w];

		TRY {
			for (i = 0; i < w; i++) {
				fp18_null(u[i]);
				fp18_new(u[i]);
			}
			fp18_new(t);

			j = 0;
			fp18_copy(t, a);
			for (i = 1; i < bn_bits(b); i++) {
				fp18_sqr_pck(t, t);
				if (bn_test_bit(b, i)) {
					fp18_copy(u[j++], t);
				}
			}

			if (!bn_is_even(b)) {
				j = 0;
				k = w - 1;
			} else {
				j = 1;
				k = w;
			}

			fp18_back_cyc_sim(u, u, k);

			if (!bn_is_even(b)) {
				fp18_copy(c, a);
			} else {
				fp18_copy(c, u[0]);
			}

			for (i = j; i < k; i++) {
				fp18_mul(c, c, u[i]);
			}
		}
		CATCH_ANY {
			THROW(ERR_CAUGHT);
		}
		FINALLY {
			for (i = 0; i < w - 1; i++) {
				fp18_free(u[i]);
			}
			fp18_free(t);
		}
	}
}

void fp18_exp_cyc_sps(fp18_t c, fp18_t a, int *b, int len) {
	int i, j, k, w = len;
	fp18_t t, u[w];

	fp18_null(t);

	TRY {
		for (i = 0; i < w; i++) {
			fp18_null(u[i]);
			fp18_new(u[i]);
		}
		fp18_new(t);

		fp18_copy(t, a);
		if (b[0] == 0) {
			for (j = 0, i = 1; i < len; i++) {
				k = (b[i] < 0 ? -b[i] : b[i]);
				for (; j < k; j++) {
					fp18_sqr_pck(t, t);
				}
				if (b[i] < 0) {
					fp18_inv_uni(u[i - 1], t);
				} else {
					fp18_copy(u[i - 1], t);
				}
			}

			fp18_back_cyc_sim(u, u, w - 1);

			fp18_copy(c, a);
			for (i = 0; i < w - 1; i++) {
				fp18_mul(c, c, u[i]);
			}
		} else {
			for (j = 0, i = 0; i < len; i++) {
				k = (b[i] < 0 ? -b[i] : b[i]);
				for (; j < k; j++) {
					fp18_sqr_pck(t, t);
				}
				if (b[i] < 0) {
					fp18_inv_uni(u[i], t);
				} else {
					fp18_copy(u[i], t);
				}
			}

			fp18_back_cyc_sim(u, u, w);

			fp18_copy(c, u[0]);
			for (i = 1; i < w; i++) {
				fp18_mul(c, c, u[i]);
			}
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		for (i = 0; i < w - 1; i++) {
			fp18_free(u[i]);
		}
		fp18_free(t);
	}
}

void fp18_conv_uni(fp18_t c, fp18_t a) {
	fp18_t t;

	fp18_null(t);

	TRY {
		fp18_new(t);

		/* t = a^{-1}. */
		fp18_inv(t, a);
		/* c = a^(p^9). */
		fp18_inv_uni(c, a);
		/* c = a^(p^9 - 1). */
		fp18_mul(c, c, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp18_free(t);
	}
}

void fp18_conv_cyc(fp18_t c, fp18_t a) {
	fp18_t t;

	fp18_null(t);

	TRY {
		fp18_new(t);

		/* First, compute c = a^(p^9 - 1). */
		/* t = a^{-1}. */
		fp18_inv(t, a);
		/* c = a^(p^9). */
		fp18_inv_uni(c, a);
		/* c = a^(p^9 - 1). */
		fp18_mul(c, c, t);

		/* Second, compute c^(p^3 + 1). */
		/* t = c^(p^3). */
		fp18_frb(t, c, 3);

		/* c = c^(p^3 + 1). */
		fp18_mul(c, c, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp18_free(t);
	}
}

int fp18_test_cyc(fp18_t a) {
	fp18_t t;
	int result = 0;

	fp18_null(t);

	TRY {
		fp18_new(t);

		fp18_back_cyc(t, a);
		result = ((fp18_cmp(t, a) == CMP_EQ) ? 1 : 0);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp18_free(t);
	}

	return result;
}

void fp18_back_cyc(fp18_t c, fp18_t a) {
	fp3_t t0, t1, t2, t3, t4, t5, t6;

	fp3_null(t0);
	fp3_null(t1);
	fp3_null(t2);
	fp3_null(t3);
	fp3_null(t4);
	fp3_null(t5);
	fp3_null(t6);

	TRY {
		fp3_new(t0);
		fp3_new(t1);
		fp3_new(t2);
		fp3_new(t3);
		fp3_new(t4);
		fp3_new(t5);
		fp3_new(t6);

		/* t3 = g3, t4 = g4, t5 = g5. */
		fp_copy(t3[0], a[1][1][0]);
		fp_copy(t3[1], a[1][0][1]);
		fp_copy(t3[2], a[1][2][1]);
		fp_copy(t4[0], a[2][0][0]);
		fp_copy(t4[1], a[2][2][0]);
		fp_copy(t4[2], a[2][1][1]);
		fp_copy(t5[0], a[2][1][0]);
		fp_copy(t5[1], a[2][0][1]);
		fp_copy(t5[2], a[2][2][1]);

		/* t0 = g4^2. */
		fp3_sqr(t0, t4);
		/* t1 = 3 * g4^2 - 2 * g3. */
		fp3_sub(t1, t0, t3);
		fp3_dbl(t1, t1);
		fp3_add(t1, t1, t0);
		/* t0 = E * g5^2 + t1. */
		fp3_sqr(t2, t5);
		fp3_mul_nor(t0, t2);
		fp3_add(t0, t0, t1);
		/* t1 = 1/(4 * g2). */
		fp_dbl(t1[0], a[1][0][0]);
		fp_dbl(t1[1], a[1][2][0]);
		fp_dbl(t1[2], a[1][1][1]);
		fp3_dbl(t1, t1);
		fp3_inv(t1, t1);
		/* t6 = g1. */
		fp3_mul(t6, t0, t1);

		/* t1 = g3 * g4, t3 = g2. */
		fp3_mul(t1, t3, t4);
		fp_copy(t3[0], a[1][0][0]);
		fp_copy(t3[1], a[1][2][0]);
		fp_copy(t3[2], a[1][1][1]);
		/* t2 = 2 * g1^2 - 3 * g3 * g4. */
		fp3_sqr(t2, t6);
		fp3_sub(t2, t2, t1);
		fp3_dbl(t2, t2);
		fp3_sub(t2, t2, t1);
		/* t1 = g2 * g5. */
		fp3_mul(t1, t3, t5);
		/* t5 = E * (2 * g1^2 + g2 * g5 - 3 * g3 * g4) + 1. */
		fp3_add(t2, t2, t1);
		fp3_mul_nor(t5, t2);
		fp_add_dig(t5[0], t5[0], 1);

		fp18_copy(c, a);
		fp_copy(c[0][0][0], t5[0]);
		fp_copy(c[0][2][0], t5[1]);
		fp_copy(c[0][1][1], t5[2]);
		fp_copy(c[0][1][0], t6[0]);
		fp_copy(c[0][0][1], t6[1]);
		fp_copy(c[0][2][1], t6[2]);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp3_free(t0);
		fp3_free(t1);
		fp3_free(t2);
		fp3_free(t3);
		fp3_free(t4);
		fp3_free(t5);
		fp3_free(t6);
	}
}

void fp18_back_cyc_sim(fp18_t c[], fp18_t a[], int n) {
	fp3_t t0[n], t1[n], t2[n], t3[n], t4[n], t5[n], t6[n];

	for (int i = 0; i < n; i++) {
		fp3_null(t0[i]);
		fp3_null(t1[i]);
		fp3_null(t2[i]);
		fp3_null(t3[i]);
		fp3_null(t4[i]);
		fp3_null(t5[i]);
		fp3_null(t6[i]);
	}

	TRY {
		for (int i = 0; i < n; i++) {
			fp3_new(t0[i]);
			fp3_new(t1[i]);
			fp3_new(t2[i]);
			fp3_new(t3[i]);
			fp3_new(t4[i]);
			fp3_new(t5[i]);
			fp3_new(t6[i]);
		}

		for (int i = 0; i < n; i++) {
			/* t3 = g3, t4 = g4, t5 = g5. */
			fp_copy(t3[i][0], a[i][1][1][0]);
			fp_copy(t3[i][1], a[i][1][0][1]);
			fp_copy(t3[i][2], a[i][1][2][1]);
			fp_copy(t4[i][0], a[i][2][0][0]);
			fp_copy(t4[i][1], a[i][2][2][0]);
			fp_copy(t4[i][2], a[i][2][1][1]);
			fp_copy(t5[i][0], a[i][2][1][0]);
			fp_copy(t5[i][1], a[i][2][0][1]);
			fp_copy(t5[i][2], a[i][2][2][1]);

			/* t0 = g4^2. */
			fp3_sqr(t0[i], t4[i]);
			/* t1 = 3 * g4^2 - 2 * g3. */
			fp3_sub(t1[i], t0[i], t3[i]);
			fp3_dbl(t1[i], t1[i]);
			fp3_add(t1[i], t1[i], t0[i]);
			/* t0 = E * g5^2 + t1. */
			fp3_sqr(t2[i], t5[i]);
			fp3_mul_nor(t0[i], t2[i]);
			fp3_add(t0[i], t0[i], t1[i]);
			/* t1 = (4 * g2). */
			fp_dbl(t1[i][0], a[i][1][0][0]);
			fp_dbl(t1[i][1], a[i][1][2][0]);
			fp_dbl(t1[i][2], a[i][1][1][1]);
			fp3_dbl(t1[i], t1[i]);
		}

		/* t1 = 1 / t1. */
		fp3_inv_sim(t1, t1, n);

		for (int i = 0; i < n; i++) {
			/* t0 = g1. */
			fp3_mul(t6[i], t0[i], t1[i]);

			/* t1 = g3 * g4. */
			fp3_mul(t1[i], t3[i], t4[i]);
			fp_copy(t3[i][0], a[i][1][0][0]);
			fp_copy(t3[i][1], a[i][1][2][0]);
			fp_copy(t3[i][2], a[i][1][1][1]);
			/* t2 = 2 * g1^2 - 3 * g3 * g4. */
			fp3_sqr(t2[i], t6[i]);
			fp3_sub(t2[i], t2[i], t1[i]);
			fp3_dbl(t2[i], t2[i]);
			fp3_sub(t2[i], t2[i], t1[i]);
			/* t1 = g2 * g5. */
			fp3_mul(t1[i], t3[i], t5[i]);
			/* t2 = E * (2 * g1^2 + g2 * g5 - 3 * g3 * g4) + 1. */
			fp3_add(t2[i], t2[i], t1[i]);
			fp3_mul_nor(t5[i], t2[i]);
			fp_add_dig(t5[i][0], t5[i][0], 1);

			fp18_copy(c[i], a[i]);
			fp_copy(c[i][0][0][0], t5[i][0]);
			fp_copy(c[i][0][2][0], t5[i][1]);
			fp_copy(c[i][0][1][1], t5[i][2]);
			fp_copy(c[i][0][1][0], t6[i][0]);
			fp_copy(c[i][0][0][1], t6[i][1]);
			fp_copy(c[i][0][2][1], t6[i][2]);
		}
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		for (int i = 0; i < n; i++) {
			fp3_free(t0[i]);
			fp3_free(t1[i]);
			fp3_free(t2[i]);
			fp3_free(t3[i]);
			fp3_free(t4[i]);
			fp3_free(t5[i]);
			fp3_free(t6[i]);
		}
	}
}
