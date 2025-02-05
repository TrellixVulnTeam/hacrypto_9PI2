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
 * Implementation of the point multiplication on prime elliptic curves.
 *
 * @version $Id: relic_ep_mul.c 1572 2013-09-02 07:11:29Z dfaranha $
 * @ingroup eb
 */

#include "relic_core.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

#if EP_MUL == LWNAF || !defined(STRIP)

#if defined(EP_KBLTZ)

static void ep_mul_glv_imp(ep_t r, const ep_t p, const bn_t k) {
	int l, l0, l1, i, n0, n1, s0, s1;
	signed char naf0[FP_BITS + 1], naf1[FP_BITS + 1], *t0, *t1;
	bn_t n, k0, k1, v1[3], v2[3];
	ep_t q, t[1 << (EP_WIDTH - 2)];

	bn_null(n);
	bn_null(k0);
	bn_null(k1);
	ep_null(q);

	TRY {
		bn_new(n);
		bn_new(k0);
		bn_new(k1);
		ep_new(q);
		for (i = 0; i < (1 << (EP_WIDTH - 2)); i++) {
			ep_null(t[i]);
			ep_new(t[i]);
		}
		for (i = 0; i < 3; i++) {
			bn_null(v1[i]);
			bn_null(v2[i]);
			bn_new(v1[i]);
			bn_new(v2[i]);
		}

		ep_curve_get_ord(n);
		ep_curve_get_v1(v1);
		ep_curve_get_v2(v2);
		bn_rec_glv(k0, k1, k, n, (const bn_t *)v1, (const bn_t *)v2);
		s0 = bn_sign(k0);
		s1 = bn_sign(k1);
		bn_abs(k0, k0);
		bn_abs(k1, k1);

		if (s0 == BN_POS) {
			ep_tab(t, p, EP_WIDTH);
		} else {
			ep_neg(q, p);
			ep_tab(t, q, EP_WIDTH);
		}

		l0 = l1 = FP_BITS + 1;
		bn_rec_naf(naf0, &l0, k0, EP_WIDTH);
		bn_rec_naf(naf1, &l1, k1, EP_WIDTH);

		l = MAX(l0, l1);
		t0 = naf0 + l - 1;
		t1 = naf1 + l - 1;
		for (i = l0; i < l; i++)
			naf0[i] = 0;
		for (i = l1; i < l; i++)
			naf1[i] = 0;

		ep_set_infty(r);
		for (i = l - 1; i >= 0; i--, t0--, t1--) {
			ep_dbl(r, r);

			n0 = *t0;
			n1 = *t1;
			if (n0 > 0) {
				ep_add(r, r, t[n0 / 2]);
			}
			if (n0 < 0) {
				ep_sub(r, r, t[-n0 / 2]);
			}
			if (n1 > 0) {
				ep_copy(q, t[n1 / 2]);
				fp_mul(q->x, q->x, ep_curve_get_beta());
				if (s0 != s1) {
					ep_neg(q, q);
				}
				ep_add(r, r, q);
			}
			if (n1 < 0) {
				ep_copy(q, t[-n1 / 2]);
				fp_mul(q->x, q->x, ep_curve_get_beta());
				if (s0 != s1) {
					ep_neg(q, q);
				}
				ep_sub(r, r, q);
			}
		}
		/* Convert r to affine coordinates. */
		ep_norm(r, r);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(n);
		bn_free(k0);
		bn_free(k1);
		bn_free(n)
		ep_free(q);
		for (i = 0; i < 1 << (EP_WIDTH - 2); i++) {
			ep_free(t[i]);
		}
		for (i = 0; i < 3; i++) {
			bn_free(v1[i]);
			bn_free(v2[i]);
		}

	}
}

#endif /* EP_KBLTZ */

#if defined(EP_ORDIN) || defined(EP_SUPER)

static void ep_mul_naf_imp(ep_t r, const ep_t p, const bn_t k) {
	int l, i, n;
	signed char naf[FP_BITS + 1], *_k;
	ep_t t[1 << (EP_WIDTH - 2)];

	for (i = 0; i < (1 << (EP_WIDTH - 2)); i++) {
		ep_null(t[i]);
	}

	TRY {
		/* Prepare the precomputation table. */
		for (i = 0; i < (1 << (EP_WIDTH - 2)); i++) {
			ep_new(t[i]);
		}
		/* Compute the precomputation table. */
		ep_tab(t, p, EP_WIDTH);

		/* Compute the w-NAF representation of k. */
		l = FP_BITS + 1;
		bn_rec_naf(naf, &l, k, EP_WIDTH);

		_k = naf + l - 1;

		ep_set_infty(r);
		for (i = l - 1; i >= 0; i--, _k--) {
			ep_dbl(r, r);

			n = *_k;
			if (n > 0) {
				ep_add(r, r, t[n / 2]);
			}
			if (n < 0) {
				ep_sub(r, r, t[-n / 2]);
			}
		}
		/* Convert r to affine coordinates. */
		ep_norm(r, r);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		/* Free the precomputation table. */
		for (i = 0; i < (1 << (EP_WIDTH - 2)); i++) {
			ep_free(t[i]);
		}
	}
}

#endif /* EP_ORDIN || EP_SUPER */
#endif /* EP_MUL == LWNAF */

#if EP_MUL == LWREG || !defined(STRIP)

#if defined(EP_ORDIN) || defined(EP_SUPER)

static void ep_mul_reg_imp(ep_t r, const ep_t p, const bn_t k) {
	int l, i, j, n;
	signed char reg[CEIL(FP_BITS + 1, EP_WIDTH - 1)], *_k;
	ep_t t[1 << (EP_WIDTH - 2)];

	for (i = 0; i < (1 << (EP_WIDTH - 2)); i++) {
		ep_null(t[i]);
	}

	TRY {
		/* Prepare the precomputation table. */
		for (i = 0; i < (1 << (EP_WIDTH - 2)); i++) {
			ep_new(t[i]);
		}
		/* Compute the precomputation table. */
		ep_tab(t, p, EP_WIDTH);

		/* Compute the w-NAF representation of k. */
		l = CEIL(FP_BITS + 1, EP_WIDTH - 1);
		bn_rec_reg(reg, &l, k, FP_BITS, EP_WIDTH);

		_k = reg + l - 1;

		ep_set_infty(r);
		for (i = l - 1; i >= 0; i--, _k--) {
			for (j = 0; j < EP_WIDTH - 1; j++) {
				ep_dbl(r, r);
			}

			n = *_k;
			if (n > 0) {
				ep_add(r, r, t[n / 2]);
			}
			if (n < 0) {
				ep_sub(r, r, t[-n / 2]);
			}
		}

		/* Convert r to affine coordinates. */
		ep_norm(r, r);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		/* Free the precomputation table. */
		for (i = 0; i < (1 << (EP_WIDTH - 2)); i++) {
			ep_free(t[i]);
		}
	}
}

#endif /* EP_ORDIN || EP_SUPER */
#endif /* EP_MUL == LWNAF */

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

#if EP_MUL == BASIC || !defined(STRIP)

void ep_mul_basic(ep_t r, const ep_t p, const bn_t k) {
	int i, l;
	ep_t t;

	ep_null(t);

	if (bn_is_zero(k)) {
		ep_set_infty(r);
		return;
	}

	TRY {
		ep_new(t);
		l = bn_bits(k);

		ep_copy(t, p);
		for (i = l - 2; i >= 0; i--) {
			ep_dbl(t, t);
			if (bn_test_bit(k, i)) {
				ep_add(t, t, p);
			}
		}

		ep_norm(r, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		ep_free(t);
	}
}

#endif

#if EP_MUL == SLIDE || !defined(STRIP)

void ep_mul_slide(ep_t r, const ep_t p, const bn_t k) {
	ep_t t[1 << (EP_WIDTH - 1)], q;
	int i, j, l;
	unsigned char win[FP_BITS + 1];

	ep_null(q);

	/* Initialize table. */
	for (i = 0; i < (1 << (EP_WIDTH - 1)); i++) {
		ep_null(t[i]);
	}

	TRY {
		for (i = 0; i < (1 << (EP_WIDTH - 1)); i ++) {
			ep_new(t[i]);
		}

		ep_new(q);

		ep_copy(t[0], p);
		ep_dbl(q, p);

#if defined(EP_MIXED)
		ep_norm(q, q);
#endif

		/* Create table. */
		for (i = 1; i < (1 << (EP_WIDTH - 1)); i++) {
			ep_add(t[i], t[i - 1], q);
		}

#if defined(EP_MIXED)
		ep_norm_sim(t + 1, (const ep_t *)t + 1, (1 << (EP_WIDTH - 1)) - 1);
#endif

		ep_set_infty(q);
		l = FP_BITS + 1;
		bn_rec_slw(win, &l, k, EP_WIDTH);
		for (i = 0; i < l; i++) {
			if (win[i] == 0) {
				ep_dbl(q, q);
			} else {
				for (j = 0; j < util_bits_dig(win[i]); j++) {
					ep_dbl(q, q);
				}
				ep_add(q, q, t[win[i] >> 1]);
			}
		}

		ep_norm(r, q);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		for (i = 0; i < (1 << (EP_WIDTH - 1)); i++) {
			ep_free(t[i]);
		}
		ep_free(q);
	}
}

#endif

#if EP_MUL == MONTY || !defined(STRIP)

void ep_mul_monty(ep_t r, const ep_t p, const bn_t k) {
	ep_t t[2];
	dig_t buf;
	int bitcnt, digidx, j;

	ep_null(t[0]);
	ep_null(t[1]);

	TRY {
		ep_new(t[0]);
		ep_new(t[1]);

		ep_set_infty(t[0]);
		ep_copy(t[1], p);

		/* Set initial mode and bitcnt, */
		bitcnt = 1;
		buf = 0;
		digidx = k->used - 1;

		for (;;) {
			/* Grab next digit as required. */
			if (--bitcnt == 0) {
				/* If digidx == -1 we are out of digits so break. */
				if (digidx == -1) {
					break;
				}
				/* Read next digit and reset bitcnt. */
				buf = k->dp[digidx--];
				bitcnt = (int)BN_DIGIT;
			}

			/* Grab the next msb from the exponent. */
			j = (buf >> (BN_DIGIT - 1)) & 0x01;
			buf <<= (dig_t)1;

			ep_add(t[j ^ 1], t[0], t[1]);
			ep_dbl(t[j], t[j]);
		}

		ep_norm(r, t[0]);

	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		ep_free(t[1]);
		ep_free(t[0]);
	}
}

#endif

#if EP_MUL == LWNAF || !defined(STRIP)

void ep_mul_lwnaf(ep_t r, const ep_t p, const bn_t k) {
#if defined(EP_KBLTZ)
	if (ep_curve_is_kbltz()) {
		ep_mul_glv_imp(r, p, k);
		return;
	}
#endif

#if defined(EP_ORDIN) || defined(EP_SUPER)
	ep_mul_naf_imp(r, p, k);
#endif
}

#endif

#if EP_MUL == LWREG || !defined(STRIP)

void ep_mul_lwreg(ep_t r, const ep_t p, const bn_t k) {
#if defined(EP_KBLTZ)
	if (ep_curve_is_kbltz()) {
		ep_mul_glv_imp(r, p, k);
		return;
	}
#endif

#if defined(EP_ORDIN) || defined(EP_SUPER)
	ep_mul_reg_imp(r, p, k);
#endif
}

#endif

void ep_mul_gen(ep_t r, const bn_t k) {
#ifdef EP_PRECO
	ep_mul_fix(r, ep_curve_get_tab(), k);
#else
	ep_t g;

	ep_null(g);

	TRY {
		ep_new(g);
		ep_curve_get_gen(g);
		ep_mul(r, g, k);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		ep_free(g);
	}
#endif
}

void ep_mul_dig(ep_t r, const ep_t p, dig_t k) {
	int i, l;
	ep_t t;

	ep_null(t);

	if (k == 0) {
		ep_set_infty(r);
		return;
	}

	TRY {
		ep_new(t);

		l = util_bits_dig(k);

		ep_copy(t, p);

		for (i = l - 2; i >= 0; i--) {
			ep_dbl(t, t);
			if (k & ((dig_t)1 << i)) {
				ep_add(t, t, p);
			}
		}

		ep_norm(r, t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		ep_free(t);
	}
}
