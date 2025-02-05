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
 * Implementation of utilities for prime elliptic curves over quadratic
 * extensions.
 *
 * @version $Id: relic_ep2_util.c 1355 2013-01-27 02:28:52Z dfaranha $
 * @ingroup epx
 */

#include "relic_core.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

int ep2_is_infty(ep2_t p) {
	return (fp2_is_zero(p->z) == 1);
}

void ep2_set_infty(ep2_t p) {
	fp2_zero(p->x);
	fp2_zero(p->y);
	fp2_zero(p->z);
}

void ep2_copy(ep2_t r, ep2_t p) {
	fp2_copy(r->x, p->x);
	fp2_copy(r->y, p->y);
	fp2_copy(r->z, p->z);
	r->norm = p->norm;
}

int ep2_cmp(ep2_t p, ep2_t q) {
	if (fp2_cmp(p->x, q->x) != CMP_EQ) {
		return CMP_NE;
	}

	if (fp2_cmp(p->y, q->y) != CMP_EQ) {
		return CMP_NE;
	}

	if (fp2_cmp(p->z, q->z) != CMP_EQ) {
		return CMP_NE;
	}

	return CMP_EQ;
}

void ep2_rand(ep2_t p) {
	bn_t n, k;
	ep2_t gen;

	bn_null(k);
	bn_null(n);
	ep2_null(gen);

	TRY {
		bn_new(k);
		bn_new(n);
		ep2_new(gen);

		ep2_curve_get_ord(n);

		bn_rand(k, BN_POS, bn_bits(n));
		bn_mod(k, k, n);

		ep2_curve_get_gen(gen);
		ep2_mul(p, gen, k);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(k);
		bn_free(n);
		ep2_free(gen);
	}
}

void ep2_rhs(fp2_t rhs, ep2_t p) {
	fp2_t t0;
	fp2_t t1;

	fp2_null(t0);
	fp2_null(t1);

	TRY {
		fp2_new(t0);
		fp2_new(t1);

		/* t0 = x1^2. */
		fp2_sqr(t0, p->x);
		/* t1 = x1^3. */
		fp2_mul(t1, t0, p->x);

		ep2_curve_get_a(t0);
		fp2_mul(t0, p->x, t0);
		fp2_add(t1, t1, t0);

		ep2_curve_get_b(t0);
		fp2_add(t1, t1, t0);

		fp2_copy(rhs, t1);

	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		fp2_free(t1);
	}
}

void ep2_tab(ep2_t * t, ep2_t p, int w) {
	if (w > 2) {
		ep2_dbl(t[0], p);
#if defined(EP_MIXED)
		ep2_norm(t[0], t[0]);
#endif
		ep2_add(t[1], t[0], p);
		for (int i = 2; i < (1 << (w - 2)); i++) {
			ep2_add(t[i], t[i - 1], t[0]);
		}
#if defined(EP_MIXED)
		for (int i = 1; i < (1 << (EP_WIDTH - 2)); i++) {
			ep2_norm(t[i], t[i]);
		}
#endif
	}
	ep2_copy(t[0], p);
}

int ep2_is_valid(ep2_t p) {
	ep2_t t;
	int r = 0;

	ep2_null(t);

	TRY {
		ep2_new(t);

		ep2_norm(t, p);

		ep2_rhs(t->x, t);
		fp2_sqr(t->y, t->y);

		r = (fp2_cmp(t->x, t->y) == CMP_EQ) || ep2_is_infty(p);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		ep2_free(t);
	}
	return r;
}

void ep2_print(ep2_t p) {
	fp2_print(p->x);
	fp2_print(p->y);
	fp2_print(p->z);
}
