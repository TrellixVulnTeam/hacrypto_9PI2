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
 * Implementation of frobenius action on prime elliptic curves over
 * quadratic extensions.
 *
 * @version $Id: relic_ep2_frb.c 1355 2013-01-27 02:28:52Z dfaranha $
 * @ingroup epx
 */

#include "relic_core.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void ep2_frb(ep2_t r, ep2_t p, int i) {
	switch (i) {
		case 1:
			fp2_frb(r->x, p->x, 1);
			fp2_frb(r->y, p->y, 1);
			if (ep2_curve_is_twist() == EP_MTYPE) {
				fp2_mul_frb(r->x, r->x, 1, 4);
				fp2_mul_art(r->x, r->x);
				fp2_mul_art(r->y, r->y);
			} else {
				fp2_mul_frb(r->x, r->x, 1, 2);
			}
			fp2_mul_frb(r->y, r->y, 1, 3);
			break;
		case 2:
			if (ep2_curve_is_twist() == EP_MTYPE) {
				fp2_mul_frb(r->x, p->x, 2, 4);
			} else {
				fp2_mul_frb(r->x, p->x, 2, 2);
			}
			fp2_neg(r->y, p->y);
			break;
		case 3:
			if (ep2_curve_is_twist() == EP_MTYPE) {
				fp2_frb(r->x, p->x, 1);
				fp2_frb(r->y, p->y, 1);
				fp2_mul_frb(r->x, r->x, 1, 4);
				fp2_mul_frb(r->x, r->x, 2, 4);
				fp2_mul_art(r->x, r->x);
				fp2_mul_frb(r->y, r->y, 1, 3);
				fp2_mul_art(r->y, r->y);
				fp2_neg(r->y, r->y);
			} else {
				fp2_frb(r->x, p->x, 1);
				fp2_mul_frb(r->x, r->x, 3, 2);
				fp_neg(r->y[0], p->y[0]);
				fp_copy(r->y[1], p->y[1]);
				fp2_mul_frb(r->y, r->y, 1, 3);
			}
			break;
	}
	r->norm = 1;
	fp_set_dig(r->z[0], 1);
	fp_zero(r->z[1]);
}
