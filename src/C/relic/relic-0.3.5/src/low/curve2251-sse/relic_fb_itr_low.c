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
 * Implementation of the low-level iterated squaring/square-root.
 *
 * @version $Id: relic_fb_itr_low.c 1355 2013-01-27 02:28:52Z dfaranha $
 * @ingroup fb
 */

#include <tmmintrin.h>
#include <xmmintrin.h>

#include "relic_fb.h"
#include "relic_dv.h"
#include "relic_fb_low.h"
#include "relic_util.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void fb_itrn_low(dig_t *c, dig_t *a, dig_t *t) {
	int i, j;
	dig_t u, *tmp, *p;

	__m128i r0, r1;
	r0 = r1 = _mm_setzero_si128();
	for (i = FB_DIGIT - 4; i >= 0; i -= 4) {
		tmp = a;
		for (j = 0; j < FB_DIGS - 1; j++, tmp++) {
			u = (*tmp >> i) & 0x0F;
			p = (t + ((j * FB_DIGIT + i) * 4 + u) * FB_DIGS);
			r0 = _mm_xor_si128(r0, *(__m128i *)(p));
			r1 = _mm_xor_si128(r1, *(__m128i *)(p + 2));
		}
	}
	for (i = FB_DIGIT - 8; i >= 0; i -= 4) {
		tmp = a + FB_DIGS - 1;
		u = (*tmp >> i) & 0x0F;
		p = (t + ((j * FB_DIGIT + i) * 4 + u) * FB_DIGS);
		r0 = _mm_xor_si128(r0, *(__m128i *)(p));
		r1 = _mm_xor_si128(r1, *(__m128i *)(p + 2));
	}

	_mm_store_si128((__m128i *)c, r0);
	_mm_store_si128((__m128i *)(c + 2), r1);
}
