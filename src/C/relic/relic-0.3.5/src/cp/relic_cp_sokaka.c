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
 * Implementation of the Sakai-Ohgishi-Kasahara Identity-Based Non-Interactive
 * Authenticated Key Agreement scheme.
 *
 * @version $Id: relic_cp_sokaka.c 1594 2013-09-03 14:08:13Z dfaranha $
 * @ingroup test
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "relic.h"
#include "relic_test.h"
#include "relic_bench.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void cp_sokaka_gen(bn_t master) {
	bn_t n;

	bn_null(n);

	TRY {
		bn_new(n);

		g1_get_ord(n);

		do {
			bn_rand(master, BN_POS, bn_bits(n));
			bn_mod(master, master, n);
		} while (bn_is_zero(master));
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(n);
	}
}

void cp_sokaka_gen_prv(sokaka_t k, char *id, int len, bn_t master) {
	if (pc_map_is_type1()) {
		g1_map(k->s1, (unsigned char *)id, len);
		g1_mul(k->s1, k->s1, master);
	} else {
		g1_map(k->s1, (unsigned char *)id, len);
		g1_mul(k->s1, k->s1, master);
		g2_map(k->s2, (unsigned char *)id, len);
		g2_mul(k->s2, k->s2, master);
	}
}

void cp_sokaka_key(unsigned char *key, unsigned int key_len, char *id1,
		int len1, sokaka_t k, char *id2, int len2) {
	int l, first = 0;
	g1_t p;
	g2_t q;
	gt_t e;
	bn_t n;

	g1_null(p);
	g2_null(q);
	gt_null(e);
	bn_null(n);

	TRY {
		g1_new(p);
		g2_new(q);
		gt_new(e);
		bn_new(n);

		if (len1 == len2) {
			if (strncmp(id1, id2, len1) == 0) {
				THROW(ERR_NO_VALID);
			}
			first = (strncmp(id1, id2, len1) == -1 ? 1 : 2);
		} else {
			if (len1 < len2) {
				if (strncmp(id1, id2, len1) == 0) {
					first = 1;
				} else {
					first = (strncmp(id1, id2, len2) == -1 ? 1 : 2);
				}
			} else {
				if (strncmp(id1, id2, len2) == 0) {
					first = 2;
				} else {
					first = (strncmp(id1, id2, len2) == -1 ? 1 : 2);
				}
			}
		}
		if (pc_map_is_type1()) {
			g2_map(q, (unsigned char *)id2, len2);
			pc_map(e, k->s1, q);
		} else {
			if (first == 1) {
				g2_map(q, (unsigned char *)id2, len2);
				pc_map(e, k->s1, q);
			} else {
				g1_map(p, (unsigned char *)id2, len2);
				pc_map(e, p, k->s2);
			}
		}
#if PC_CUR == PRIME

#if FP_PRIME < 1536
		unsigned char buf[12 * FP_BYTES], *ptr;
		ptr = buf;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 3; j++) {
				for (int m = 0; m < 2; m++) {
					fp_prime_back(n, e[i][j][m]);
					bn_size_bin(&l, n);
					bn_write_bin(ptr, FP_BYTES, n);
					ptr += FP_BYTES;
				}
			}
		}
#else
		unsigned char buf[2 * FP_BYTES], *ptr;
		ptr = buf;
		for (int i = 0; i < 2; i++) {
			fp_prime_back(n, e[i]);
			bn_size_bin(&l, n);
			bn_write_bin(ptr, FP_BYTES, n);
			ptr += FP_BYTES;
		}
#endif
#else
		unsigned char buf[4 * FB_BYTES], *ptr;
		ptr = buf;
		for (int i = 0; i < 4; i++) {
			memcpy(ptr, e[i], FB_BYTES);
			ptr += FB_BYTES;
		}
#endif
		md_kdf1(key, key_len, buf, sizeof(buf));
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		g1_free(p);
		g2_free(q);
		gt_free(e);
		bn_free(n);
	}
}
