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
 * Implementation of the final exponentiation for pairings over prime curves.
 *
 * @version $Id: relic_pp_exp.c 1591 2013-09-03 08:20:25Z dfaranha $
 * @ingroup pp
 */

#include "relic_core.h"
#include "relic_pp.h"
#include "relic_util.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

/**
 * Computes the final exponentiation of a pairing defined over a Barreto-Naehrig
 * curve.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the extension field element to exponentiate.
 */
static void pp_exp_bn(fp12_t c, fp12_t a) {
	fp12_t t0, t1, t2, t3;
	int l = MAX_TERMS + 1, b[MAX_TERMS + 1];
	bn_t x;

	fp12_null(t0);
	fp12_null(t1);
	fp12_null(t2);
	fp12_null(t3);
	bn_null(x);

	TRY {
		fp12_new(t0);
		fp12_new(t1);
		fp12_new(t2);
		fp12_new(t3);
		bn_new(x);

		/*
		 * New final exponentiation following Fuentes-Casta�eda, Knapp and
		 * Rodr�guez-Henr�quez: Fast Hashing to G_2.
		 */
		fp_param_get_var(x);
		fp_param_get_sps(b, &l);

		/* First, compute m = f^(p^6 - 1)(p^2 + 1). */
		fp12_conv_cyc(c, a);

		/* Now compute m^((p^4 - p^2 + 1) / r). */
		/* t0 = m^2x. */
		fp12_exp_cyc_sps(t0, c, b, l);
		fp12_sqr_cyc(t0, t0);
		/* t1 = m^6x. */
		fp12_sqr_cyc(t1, t0);
		fp12_mul(t1, t1, t0);
		/* t2 = m^6x^2. */
		fp12_exp_cyc_sps(t2, t1, b, l);
		/* t3 = m^12x^3. */
		fp12_sqr_cyc(t3, t2);
		fp12_exp_cyc_sps(t3, t3, b, l);

		if (bn_sign(x) == BN_NEG) {
			fp12_inv_uni(t0, t0);
			fp12_inv_uni(t1, t1);
			fp12_inv_uni(t3, t3);
		}

		/* t3 = a = m^12x^3 * m^6x^2 * m^6x. */
		fp12_mul(t3, t3, t2);
		fp12_mul(t3, t3, t1);

		/* t0 = b = 1/(m^2x) * t3. */
		fp12_inv_uni(t0, t0);
		fp12_mul(t0, t0, t3);

		/* Compute t2 * t3 * m * b^p * a^p^2 * [b * 1/m]^p^3. */
		fp12_mul(t2, t2, t3);
		fp12_mul(t2, t2, c);
		fp12_inv_uni(c, c);
		fp12_mul(c, c, t0);
		fp12_frb(c, c, 3);
		fp12_mul(c, c, t2);
		fp12_frb(t0, t0, 1);
		fp12_mul(c, c, t0);
		fp12_frb(t3, t3, 2);
		fp12_mul(c, c, t3);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp12_free(t0);
		fp12_free(t1);
		fp12_free(t2);
		fp12_free(t3);
		bn_free(x);
	}
}

/**
 * Computes the final exponentiation of a pairing defined over a
 * Barreto-Lynn-Scott curve.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the extension field element to exponentiate.
 */
static void pp_exp_b12(fp12_t c, fp12_t a) {
	fp12_t t[10];
	bn_t x;
	int l = MAX_TERMS + 1, b[MAX_TERMS + 1];

	bn_null(x);

	TRY {
		for (int i = 0; i < 10; i++) {
			fp12_null(t[i]);
			fp12_new(t[i]);
		}
		bn_new(x);

		fp_param_get_var(x);
		fp_param_get_sps(b, &l);

		/* First, compute m^(p^6 - 1)(p^2 + 1). */
		fp12_conv_cyc(c, a);

		/* v0 = f^-1. */
		fp12_inv_uni(t[0], c);

		/* v1 = f^-2. */
		fp12_sqr_cyc(t[1], t[0]);

		/* v2 = f^x. */
		fp12_exp_cyc_sps(t[2], c, b, l);

		if (bn_sign(x) == BN_NEG) {
			fp12_inv_uni(t[2], t[2]);
		}

		/* v3 = f^2x. */
		fp12_sqr_cyc(t[3], t[2]);

		/* v4 = f^(x - 2). */
		fp12_mul(t[4], t[2], t[1]);

		/* v5 = f^(x^2 - 2x). */
		fp12_exp_cyc_sps(t[5], t[4], b, l);

		if (bn_sign(x) == BN_NEG) {
			fp12_inv_uni(t[5], t[5]);
		}

		/* v6 = f^(x^3 - 2x^2). */
		fp12_exp_cyc_sps(t[6], t[5], b, l);

		if (bn_sign(x) == BN_NEG) {
			fp12_inv_uni(t[6], t[6]);
		}

		/* v7 = f^(x^4 - 2x^3 + 2x). */
		fp12_exp_cyc_sps(t[7], t[6], b, l);
		if (bn_sign(x) == BN_NEG) {
			fp12_inv_uni(t[7], t[7]);
		}
		fp12_mul(t[7], t[7], t[3]);

		/* v8 = f^(x^5 - 2x^4 + 2x^2). */
		fp12_exp_cyc_sps(t[8], t[7], b, l);
		if (bn_sign(x) == BN_NEG) {
			fp12_inv_uni(t[8], t[8]);
		}

		/* v7 = f^(x^4 - 2x^3 + 2x - 1)^p. */
		fp12_mul(t[7], t[7], t[0]);
		fp12_frb(t[7], t[7], 1);

		/* v6 = f^(x^3 - 2x^2 + x)^p^2. */
		fp12_mul(t[6], t[6], t[2]);
		fp12_frb(t[6], t[6], 2);

		/* v5 = f^(x^2 - 2x + 1)^p^3. */
		fp12_mul(t[5], t[5], c);
		fp12_frb(t[5], t[5], 1);
		fp12_frb(t[5], t[5], 2);

		/* v4 = f^(2 - x). */
		fp12_inv_uni(t[4], t[4]);

		/* Now compute f * v4 * v5 * v6 * v7 * v8. */
		fp12_mul(c, c, t[4]);
		fp12_mul(c, c, t[5]);
		fp12_mul(c, c, t[6]);
		fp12_mul(c, c, t[7]);
		fp12_mul(c, c, t[8]);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		for (int i = 0; i < 9; i++) {
			fp12_free(t[i]);
		}
		bn_free(x);
	}
}

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void pp_exp_k2(fp2_t c, fp2_t a) {
	bn_t e, n;

	bn_null(n);
	bn_null(e);

	TRY {
		bn_new(n);
		bn_new(e);

		ep_curve_get_ord(n);

		fp2_conv_uni(c, a);
		dv_copy(e->dp, fp_prime_get(), FP_DIGS);
		e->used = FP_DIGS;
		e->sign = BN_POS;
		bn_add_dig(e, e, 1);
		bn_div(e, e, n);
		fp2_exp_uni(c, c, e);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		bn_free(n);
		bn_free(e);
	}
}

void pp_exp_k12(fp12_t c, fp12_t a) {
	switch (ep_param_get()) {
		case BN_P158:
		case BN_P254:
		case BN_P256:
		case BN_P638:
			pp_exp_bn(c, a);
			break;
		case B12_P638:
			pp_exp_b12(c, a);
			break;
	}
}
