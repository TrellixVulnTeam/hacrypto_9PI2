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
 * Implementation of a Key Derivation function.
 *
 * @version $Id: relic_md_kdf.c 1566 2013-09-02 04:09:52Z dfaranha $
 * @ingroup md
 */

#include <string.h>

#include "relic_conf.h"
#include "relic_core.h"
#include "relic_util.h"
#include "relic_md.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void md_kdf1(unsigned char *key, int key_len, const unsigned char *in,
		int in_len) {
	uint32_t i, j, d;
	unsigned char buffer[in_len + sizeof(uint32_t)];
	unsigned char t[key_len + MD_LEN];

	/* d = ceil(kLen/hLen). */
	d = CEIL(key_len, MD_LEN);
	memcpy(buffer, in, in_len);
	for (i = 0; i < d; i++) {
		j = util_conv_big(i);
		/* c = integer_to_string(c, 4). */
		memcpy(buffer + in_len, &j, sizeof(uint32_t));
		/* t = t || hash(z || c). */
		md_map(t + i * MD_LEN, buffer, in_len + sizeof(uint32_t));
	}
	memcpy(key, t, key_len);
}

void md_kdf2(unsigned char *key, int key_len, const unsigned char *in,
		int in_len) {
	uint32_t i, j, d;
	unsigned char buffer[in_len + sizeof(uint32_t)];
	unsigned char t[key_len + MD_LEN];

	/* d = ceil(kLen/hLen). */
	d = CEIL(key_len, MD_LEN);
	memcpy(buffer, in, in_len);
	for (i = 1; i <= d; i++) {
		j = util_conv_big(i);
		/* c = integer_to_string(c, 4). */
		memcpy(buffer + in_len, &j, sizeof(uint32_t));
		/* t = t || hash(z || c). */
		md_map(t + (i - 1) * MD_LEN, buffer, in_len + sizeof(uint32_t));
	}
	memcpy(key, t, key_len);
}
