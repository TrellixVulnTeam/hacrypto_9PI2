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
 * Implementation of the low-level multiple precision shifting functions.
 *
 * @version $Id: relic_bn_shift_low.c 1355 2013-01-27 02:28:52Z dfaranha $
 * @ingroup bn
 */

#include "relic_bn.h"
#include "relic_bn_low.h"
#include "relic_util.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

dig_t bn_lshb_low(dig_t *c, dig_t *a, int size, int bits) {
	int i;
	dig_t r, carry, shift;

	if (bits == 1)
		return bn_lsh1_low(c, a, size);

	shift = BN_DIGIT - bits;
	carry = 0;
	for (i = 0; i < size; i++, a++, c++) {
		/* Get the needed least significant bits. */
		r = ((*a) >> shift) & MASK(bits);
		/* Shift left the operand. */
		*c = ((*a) << bits) | carry;
		/* Update the carry. */
		carry = r;
	}
	return carry;
}

void bn_lshd_low(dig_t *c, dig_t *a, int size, int digits) {
	dig_t *top, *bot;
	int i;

	top = c + size + digits - 1;
	bot = a + size - 1;

	for (i = 0; i < size; i++, top--, bot--) {
		*top = *bot;
	}
	for (i = 0; i < digits; i++, c++) {
		*c = 0;
	}
}

dig_t bn_rshb_low(dig_t *c, dig_t *a, int size, int bits) {
	int i;
	dig_t r, carry, shift;

	if (bits == 1)
		return bn_rsh1_low(c, a, size);

	c += size - 1;
	a += size - 1;
	/* Prepare the bit mask. */
	shift = BN_DIGIT - bits;
	carry = 0;
	for (i = size - 1; i >= 0; i--, a--, c--) {
		/* Get the needed least significant bits. */
		r = (*a) & MASK(bits);
		/* Shift left the operand. */
		*c = ((*a) >> bits) | (carry << shift);
		/* Update the carry. */
		carry = r;
	}
	return carry;
}

void bn_rshd_low(dig_t *c, dig_t *a, int size, int digits) {
	dig_t *top, *bot;
	int i;

	top = a + digits;
	bot = c;

	for (i = 0; i < size - digits; i++, top++, bot++) {
		*bot = *top;
	}
}
