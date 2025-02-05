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
 * Implementation of simultaneous point multiplication on binary elliptic
 * curves.
 *
 * @version $Id: relic_eb_mul_sim.c 1566 2013-09-02 04:09:52Z dfaranha $
 * @ingroup eb
 */

#include "string.h"

#include "relic_core.h"
#include "relic_eb.h"
#include "relic_util.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

#if EB_SIM == INTER || !defined(STRIP)

#if defined(EB_KBLTZ)

/**
 * Multiplies and adds two binary elliptic curve points simultaneously,
 * optionally choosing the first point as the generator depending on an optional
 * table of precomputed points.
 *
 * @param[out] r 				- the result.
 * @param[in] p					- the first point to multiply.
 * @param[in] k					- the first integer.
 * @param[in] q					- the second point to multiply.
 * @param[in] m					- the second integer.
 * @param[in] t					- the pointer to a precomputed table.
 */
static void eb_mul_sim_kbltz(eb_t r, const eb_t p, const bn_t k, const eb_t q,
		const bn_t m, const eb_t *t) {
	int l0, l1, l, i, n0, n1, w, g;
	signed char u, tnaf0[FB_BITS + 8], tnaf1[FB_BITS + 8], *_k, *_m;
	eb_t t0[1 << (EB_WIDTH - 2)];
	eb_t t1[1 << (EB_WIDTH - 2)];
	bn_t vm, s0, s1;

	bn_null(vm);
	bn_null(s0);
	bn_null(s1);

	for (i = 0; i < (1 << (EB_WIDTH - 2)); i++) {
		eb_null(t0[i]);
		eb_null(t1[i]);
	}

	TRY {
		bn_new(vm);
		bn_new(s0);
		bn_new(s1);

		/* Compute the w-TNAF representation of k. */
		if (eb_curve_opt_a() == OPT_ZERO) {
			u = -1;
		} else {
			u = 1;
		}

		g = (t == NULL ? 0 : 1);
		if (!g) {
			for (i = 0; i < (1 << (EB_WIDTH - 2)); i++) {
				eb_new(t0[i]);
				eb_set_infty(t0[i]);
				fb_set_bit(t0[i]->z, 0, 1);
				t0[i]->norm = 1;
			}
			eb_tab(t0, p, EB_WIDTH);
			t = (const eb_t *)t0;
		}

		/* Prepare the precomputation table. */
		for (i = 0; i < (1 << (EB_WIDTH - 2)); i++) {
			eb_new(t1[i]);
			eb_set_infty(t1[i]);
			fb_set_bit(t1[i]->z, 0, 1);
			t1[i]->norm = 1;
		}
		/* Compute the precomputation table. */
		eb_tab(t1, q, EB_WIDTH);

		/* Compute the w-TNAF representation of k. */
		if (g) {
			w = EB_DEPTH;
		} else {
			w = EB_WIDTH;
		}
		eb_curve_get_vm(vm);
		eb_curve_get_s0(s0);
		eb_curve_get_s1(s1);

		l0 = l1 = FB_BITS + 8;
		bn_rec_tnaf(tnaf0, &l0, k, vm, s0, s1, u, FB_BITS, w);
		bn_rec_tnaf(tnaf1, &l1, m, vm, s0, s1, u, FB_BITS, EB_WIDTH);

		l = MAX(l0, l1);
		_k = tnaf0 + l - 1;
		_m = tnaf1 + l - 1;
		for (i = l0; i < l; i++)
			tnaf0[i] = 0;
		for (i = l1; i < l; i++)
			tnaf1[i] = 0;

		_k = tnaf0 + l - 1;
		_m = tnaf1 + l - 1;
		eb_set_infty(r);
		for (i = l - 1; i >= 0; i--, _k--, _m--) {
			eb_frb(r, r);

			n0 = *_k;
			n1 = *_m;
			if (n0 > 0) {
				eb_add(r, r, t[n0 / 2]);
			}
			if (n0 < 0) {
				eb_sub(r, r, t[-n0 / 2]);
			}
			if (n1 > 0) {
				eb_add(r, r, t1[n1 / 2]);
			}
			if (n1 < 0) {
				eb_sub(r, r, t1[-n1 / 2]);
			}
		}
		/* Convert r to affine coordinates. */
		eb_norm(r, r);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		if (!g) {
			for (i = 0; i < (1 << (EB_WIDTH - 2)); i++) {
				eb_free(t0[i]);
			}
		}
		for (i = 0; i < (1 << (EB_WIDTH - 2)); i++) {
			eb_free(t1[i]);
		}
		bn_free(vm);
		bn_free(s0);
		bn_free(s1);
	}
}

#endif /* EB_KBLTZ */

#if defined(EB_ORDIN) || defined(EB_SUPER)

/**
 * Multiplies and adds two binary elliptic curve points simultaneously,
 * optionally choosing the first point as the generator depending on an optional
 * table of precomputed points.
 *
 * @param[out] r 				- the result.
 * @param[in] p					- the first point to multiply.
 * @param[in] k					- the first integer.
 * @param[in] q					- the second point to multiply.
 * @param[in] m					- the second integer.
 * @param[in] t					- the pointer to a precomputed table.
 */
static void eb_mul_sim_ordin(eb_t r, const eb_t p, const bn_t k, const eb_t q,
		const bn_t m, const eb_t *t) {
	int l, l0, l1, i, n0, n1, w, g;
	signed char naf0[FB_BITS + 1], naf1[FB_BITS + 1], *_k, *_m;
	eb_t t0[1 << (EB_WIDTH - 2)];
	eb_t t1[1 << (EB_WIDTH - 2)];

	for (i = 0; i < (1 << (EB_WIDTH - 2)); i++) {
		eb_null(t0[i]);
		eb_null(t1[i]);
	}

	TRY {
		g = (t == NULL ? 0 : 1);
		if (!g) {
			for (i = 0; i < (1 << (EB_WIDTH - 2)); i++) {
				eb_new(t0[i]);
			}
			eb_tab(t0, p, EB_WIDTH);
			t = (const eb_t *)t0;
		}

		/* Prepare the precomputation table. */
		for (i = 0; i < (1 << (EB_WIDTH - 2)); i++) {
			eb_new(t1[i]);
		}
		/* Compute the precomputation table. */
		eb_tab(t1, q, EB_WIDTH);

		/* Compute the w-NAF representation of k. */
		if (g) {
			w = EB_DEPTH;
		} else {
			w = EB_WIDTH;
		}

		l0 = l1 = FB_BITS + 1;
		bn_rec_naf(naf0, &l0, k, w);
		bn_rec_naf(naf1, &l1, m, EB_WIDTH);

		l = MAX(l0, l1);
		_k = naf0 + l - 1;
		_m = naf1 + l - 1;
		for (i = l0; i < l; i++) {
			naf0[i] = 0;
		}
		for (i = l1; i < l; i++) {
			naf1[i] = 0;
		}

		eb_set_infty(r);
		for (i = l - 1; i >= 0; i--, _k--, _m--) {
			eb_dbl(r, r);

			n0 = *_k;
			n1 = *_m;
			if (n0 > 0) {
				eb_add(r, r, t[n0 / 2]);
			}
			if (n0 < 0) {
				eb_sub(r, r, t[-n0 / 2]);
			}
			if (n1 > 0) {
				eb_add(r, r, t1[n1 / 2]);
			}
			if (n1 < 0) {
				eb_sub(r, r, t1[-n1 / 2]);
			}
		}
		/* Convert r to affine coordinates. */
		eb_norm(r, r);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		/* Free the precomputation tables. */
		if (!g) {
			for (i = 0; i < 1 << (EB_WIDTH - 2); i++) {
				eb_free(t0[i]);
			}
		}
		for (i = 0; i < 1 << (EB_WIDTH - 2); i++) {
			eb_free(t1[i]);
		}
	}
}

#endif /* EB_ORDIN || EB_SUPER */

#endif /* EB_SIM == INTER */

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

#if EB_SIM == BASIC || !defined(STRIP)

void eb_mul_sim_basic(eb_t r, const eb_t p, const bn_t k, const eb_t q,
		const bn_t m) {
	eb_t t;

	eb_null(t);

	TRY {
		eb_new(t);
		eb_mul(t, q, m);
		eb_mul(r, p, k);
		eb_add(t, t, r);
		eb_norm(r, t);

	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		eb_free(t);
	}
}

#endif

#if EB_SIM == TRICK || !defined(STRIP)

void eb_mul_sim_trick(eb_t r, const eb_t p, const bn_t k, const eb_t q,
		const bn_t m) {
	eb_t t0[1 << (EB_WIDTH / 2)], t1[1 << (EB_WIDTH / 2)], t[1 << EB_WIDTH];
	bn_t n;
	int l0, l1, w = EB_WIDTH / 2;
	unsigned char w0[CEIL(FB_BITS, 2)], w1[CEIL(FB_BITS, w)];

	bn_null(n);

	for (int i = 0; i < 1 << EB_WIDTH; i++) {
		eb_null(t[i]);
	}

	for (int i = 0; i < 1 << (EB_WIDTH / 2); i++) {
		eb_null(t0[i]);
		eb_null(t1[i]);
	}

	TRY {
		bn_new(n);

		eb_curve_get_ord(n);

		for (int i = 0; i < (1 << w); i++) {
			eb_new(t0[i]);
			eb_new(t1[i]);
		}
		for (int i = 0; i < (1 << EB_WIDTH); i++) {
			eb_new(t[i]);
		}

		eb_set_infty(t0[0]);
		for (int i = 1; i < (1 << w); i++) {
			eb_add(t0[i], t0[i - 1], p);
		}

		eb_set_infty(t1[0]);
		for (int i = 1; i < (1 << w); i++) {
			eb_add(t1[i], t1[i - 1], q);
		}

		for (int i = 0; i < (1 << w); i++) {
			for (int j = 0; j < (1 << w); j++) {
				eb_add(t[(i << w) + j], t0[i], t1[j]);
			}
		}

#if EB_WIDTH > 2 && defined(EB_MIXED)
		eb_norm_sim(t + 1, (const eb_t *)(t + 1), (1 << EB_WIDTH) - 1);
#endif

		l0 = l1 = CEIL(FB_BITS, w);
		bn_rec_win(w0, &l0, k, w);
		bn_rec_win(w1, &l1, m, w);

		for (int i = l0; i < l1; i++) {
			w0[i] = 0;
		}
		for (int i = l1; i < l0; i++) {
			w1[i] = 0;
		}

		eb_set_infty(r);
		for (int i = MAX(l0, l1) - 1; i >= 0; i--) {
			for (int j = 0; j < w; j++) {
				eb_dbl(r, r);
			}
			eb_add(r, r, t[(w0[i] << w) + w1[i]]);
		}
		eb_norm(r, r);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(n);
		for (int i = 0; i < (1 << w); i++) {
			eb_free(t0[i]);
			eb_free(t1[i]);
		}
		for (int i = 0; i < (1 << EB_WIDTH); i++) {
			eb_free(t[i]);
		}
	}
}
#endif

#if EB_SIM == INTER || !defined(STRIP)

void eb_mul_sim_inter(eb_t r, const eb_t p, const bn_t k, const eb_t q,
		const bn_t m) {
#if defined(EB_KBLTZ)
	if (eb_curve_is_kbltz()) {
		eb_mul_sim_kbltz(r, p, k, q, m, NULL);
		return;
	}
#endif

#if defined(EB_ORDIN) || defined(EB_SUPER)
	eb_mul_sim_ordin(r, p, k, q, m, NULL);
#endif
}

#endif

#if EB_SIM == JOINT || !defined(STRIP)

void eb_mul_sim_joint(eb_t r, const eb_t p, const bn_t k, const eb_t q,
		const bn_t m) {
	eb_t t[5];
	int u_i, len, offset;
	signed char jsf[2 * (FB_BITS + 1)];
	int i;

	eb_null(t[0]);
	eb_null(t[1]);
	eb_null(t[2]);
	eb_null(t[3]);
	eb_null(t[4]);

	TRY {
		for (i = 0; i < 5; i++) {
			eb_new(t[i]);
		}

		eb_set_infty(t[0]);
		eb_copy(t[1], q);
		eb_copy(t[2], p);
		eb_add(t[3], p, q);
		eb_sub(t[4], p, q);
#if defined(EB_MIXED)
		eb_norm_sim(t + 3, (const eb_t*)(t + 3), 2);
#endif

		len = 2 * (FB_BITS + 1);
		bn_rec_jsf(jsf, &len, k, m);

		eb_set_infty(r);

		offset = MAX(bn_bits(k), bn_bits(m)) + 1;
		for (i = len - 1; i >= 0; i--) {
			eb_dbl(r, r);
			if (jsf[i] != 0 && jsf[i] == -jsf[i + offset]) {
				u_i = jsf[i] * 2 + jsf[i + offset];
				if (u_i < 0) {
					eb_sub(r, r, t[4]);
				} else {
					eb_add(r, r, t[4]);
				}
			} else {
				u_i = jsf[i] * 2 + jsf[i + offset];
				if (u_i < 0) {
					eb_sub(r, r, t[-u_i]);
				} else {
					eb_add(r, r, t[u_i]);
				}
			}
		}
		eb_norm(r, r);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		for (i = 0; i < 5; i++) {
			eb_free(t[i]);
		}
	}
}

#endif

void eb_mul_sim_gen(eb_t r, const bn_t k, const eb_t q, const bn_t m) {
	eb_t g;

	eb_null(g);

	TRY {
		eb_new(g);

		eb_curve_get_gen(g);

#if defined(EB_KBLTZ)
#if EB_SIM == INTER && EB_FIX == LWNAF && defined(EB_PRECO)
		if (eb_curve_is_kbltz()) {
			eb_mul_sim_kbltz(r, g, k, q, m, eb_curve_get_tab());
		}
#else
		if (eb_curve_is_kbltz()) {
			eb_mul_sim(r, g, k, q, m);
		}
#endif
#endif

#if defined(EB_ORDIN) || defined(EB_SUPER)
#if EB_SIM == INTER && EB_FIX == LWNAF && defined(EB_PRECO)
		if (!eb_curve_is_kbltz()) {
			eb_mul_sim_ordin(r, g, k, q, m, eb_curve_get_tab());
		}
#else
		if (!eb_curve_is_kbltz()) {
			eb_mul_sim(r, g, k, q, m);
		}
#endif
#endif
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		eb_free(g);
	}
}
