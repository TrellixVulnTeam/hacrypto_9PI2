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
 * Implementation of the ECDSA protocol.
 *
 * @version $Id: relic_cp_ecdh.c 1355 2013-01-27 02:28:52Z dfaranha $
 * @ingroup cp
 */

#include "relic.h"
#include "relic_test.h"
#include "relic_bench.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void cp_ecdh_gen(bn_t d, ec_t q) {
	bn_t n;

	bn_null(n);

	TRY {
		bn_new(n);

		ec_curve_get_ord(n);

		do {
			bn_rand(d, BN_POS, bn_bits(n));
			bn_mod(d, d, n);
		} while (bn_is_zero(d));

		ec_mul_gen(q, d);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(n);
	}
}

void cp_ecdh_key(unsigned char *key, int key_len, bn_t d, ec_t q) {
	ec_t p;
	bn_t x;
	int l;
	unsigned char _x[EC_BYTES];

	ec_null(p);
	bn_null(x);

	TRY {
		ec_new(p);
		bn_new(x);

		ec_mul(p, q, d);
		ec_get_x(x, p);
		bn_size_bin(&l, x);
		bn_write_bin(_x, l, x);
		md_kdf2(key, key_len, _x, l);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		ec_free(p);
	}
}
