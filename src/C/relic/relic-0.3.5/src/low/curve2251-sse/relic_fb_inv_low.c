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
 * Implementation of the low-level inversion functions.
 *
 * @version $Id: relic_fb_inv_low.c 1355 2013-01-27 02:28:52Z dfaranha $
 * @ingroup fb
 */

#include <stdio.h>
#include "relic_fb.h"
#include "relic_fb_low.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void fb_invn_low(dig_t *c, dig_t *a) {
	int i, j, x, y;
	int *chain, len;

	chain = fb_poly_get_chain(&len);

	int u[len + 1];
	fb_t table[len + 1];
	for (i = 0; i <= len; i++) {
		fb_null(table[i]);
	}

	for (i = 0; i <= len; i++) {
		fb_new(table[i]);
	}

	u[0] = 1;
	u[1] = 2;
	fb_copy(table[0], a);
	fb_sqr(table[1], table[0]);
	fb_mul(table[1], table[1], table[0]);

	u[2] = u[1] + u[0];
	fb_sqr(table[2], table[1]);
	fb_mul(table[2], table[2], table[0]);

	u[3] = u[2] + u[1];
	fb_sqr(table[3], table[2]);
	for (j = 1; j < u[1]; j++) {
		fb_sqr(table[3], table[3]);
	}
	fb_mul(table[3], table[3], table[1]);

	u[4] = 2 * u[3];
	fb_sqr(table[4], table[3]);
	for (j = 1; j < u[3]; j++) {
		fb_sqr(table[4], table[4]);
	}
	fb_mul(table[4], table[4], table[3]);

	u[5] = u[4] + u[3];
	fb_sqr(table[5], table[4]);
	for (j = 1; j < u[3]; j++) {
		fb_sqr(table[5], table[5]);
	}
	fb_mul(table[5], table[5], table[3]);

	for (i = 6; i <= len; i++) {
		x = chain[i - 1] >> 8;
		y = chain[i - 1] - (x << 8);
		if (x == y) {
			u[i] = 2 * u[i - 1];
		} else {
			u[i] = u[x] + u[y];
		}
		dig_t *tab = (dig_t *)fb_poly_tab_sqr(y);
		fb_itr(table[i], table[x], u[y], (void *)tab);
		fb_mul(table[i], table[i], table[y]);
	}
	fb_sqr(c, table[len]);
}
