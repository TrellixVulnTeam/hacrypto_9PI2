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
 * Implementation of prime factorization algorithms.
 *
 * @version $Id: relic_bn_factor.c 1542 2013-08-28 18:31:47Z dfaranha $
 * @ingroup bn
 */

#include "relic_core.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

int bn_factor(bn_t c, const bn_t a) {
	bn_t t0, t1;
	int result;
	unsigned int i, tests;

	bn_null(t0);
	bn_null(t1);

	result = 1;

	if (bn_is_even(a)) {
		bn_set_dig(c, 2);
		return 1;
	}

	TRY {
		bn_new(t0);
		bn_new(t1);

		bn_set_dig(t0, 2);

#if WORD == 8
		tests = 255;
#else
		tests = 65535;
#endif
		for (i = 2; i < tests; i++) {
			bn_set_dig(t1, i);
			bn_mxp(t0, t0, t1, a);
		}

		bn_sub_dig(t0, t0, 1);
		bn_gcd(t1, t0, a);
		if (bn_cmp_dig(t1, 1) == CMP_GT && bn_cmp(t1, a) == CMP_LT) {
			bn_copy(c, t1);
		} else {
			result = 0;
		}
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		bn_free(t0);
		bn_free(t1);
	}
	return result;
}

int bn_is_factor(bn_t c, const bn_t a) {
	bn_t t;
	int result;

	bn_null(t);
	result = 1;

	TRY {
		bn_new(t);

		bn_mod(t, a, c);

		if (!bn_is_zero(t)) {
			result = 0;
		}
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		bn_free(t);
	}
	return result;
}
