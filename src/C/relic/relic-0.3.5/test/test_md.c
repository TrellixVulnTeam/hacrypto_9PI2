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
 * Tests for hash functions.
 *
 * @version $Id: test_md.c 1403 2013-04-30 22:34:04Z dfaranha $
 * @ingroup test
 */

#include <stdio.h>

#include "relic.h"
#include "relic_test.h"

#if ARCH == AVR || ARCH == MSP || ARCH == ARM
#define MSG_SIZE	112
#define TEST_MAX	2
#else
#define MSG_SIZE	1000001
#define TEST_MAX	3
#endif

/*
 *  Define patterns for testing
 */
#define TEST1   "abc"

#define TEST2a  "abcdbcdecdefdefgefghfghighijhi"
#define TEST2b  "jkijkljklmklmnlmnomnopnopq"

#define TEST3a	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
#define TEST3b	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"

#define TEST4a	"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
#define TEST4b	"hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"

char *tests[3] = {
	TEST1,
	TEST2a TEST2b,
	TEST3a TEST3b,
};

long int count[3] = { 1, 1, 10000 };

#if MD_MAP == SHONE || !defined(STRIP)

unsigned char result1[3][20] = {
	{0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
			0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D},
	{0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
			0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1},
	{0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
			0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F},
};

static int sha1(void) {
	int code = STS_ERR;
	int i, j;
	unsigned char message[MSG_SIZE], digest[20];

	TEST_ONCE("sha1 hash function is correct") {
		for (i = 0; i < TEST_MAX; i++) {
			message[0] = '\0';
			for (j = 0; j < count[i]; j++) {
				strcat((char *)message, tests[i]);
			}
			md_map_shone(digest, message, strlen((char *)message));
			TEST_ASSERT(memcmp(digest, result1[i], 20) == 0, end);
		}
	}
	TEST_END;

	code = STS_OK;

  end:
	return code;
}

#endif

#if MD_MAP == SH224 || !defined(STRIP)

unsigned char result224[3][28] = {
	{0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22, 0x86, 0x42, 0xA4, 0x77,
				0xBD, 0xA2, 0x55, 0xB3, 0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0,
			0xB3, 0xF7, 0xE3, 0x6C, 0x9D, 0xA7},
	{0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC, 0x5D, 0xBA, 0x5D, 0xA1,
				0xFD, 0x89, 0x01, 0x50, 0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5,
			0x8B, 0x19, 0x52, 0x52, 0x25, 0x25},
	{0x20, 0x79, 0x46, 0x55, 0x98, 0x0C, 0x91, 0xD8, 0xBB, 0xB4, 0xC1, 0xEA,
				0x97, 0x61, 0x8A, 0x4B, 0xF0, 0x3F, 0x42, 0x58, 0x19, 0x48,
			0xB2, 0xEE, 0x4E, 0xE7, 0xAD, 0x67},
};

static int sha224(void) {
	int code = STS_ERR;
	int i, j;
	unsigned char message[MSG_SIZE], digest[28];

	TEST_ONCE("sha224 hash function is correct") {
		for (i = 0; i < TEST_MAX; i++) {
			message[0] = '\0';
			for (j = 0; j < count[i]; j++) {
				strcat((char *)message, tests[i]);
			}
			md_map_sh224(digest, message, strlen((char *)message));
			TEST_ASSERT(memcmp(digest, result224[i], 28) == 0, end);
		}
	}
	TEST_END;

	code = STS_OK;

  end:
	return code;
}

#endif

#if MD_MAP == SH256 || !defined(STRIP)

unsigned char result256[3][32] = {
	{0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA, 0x41, 0x41, 0x40, 0xDE,
				0x5D, 0xAE, 0x22, 0x23, 0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17,
			0x7A, 0x9C, 0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD},
	{0x24, 0x8d, 0x6A, 0x61, 0xd2, 0x06, 0x38, 0xb8, 0xE5, 0xc0, 0x26, 0x93,
				0x0c, 0x3E, 0x60, 0x39, 0xA3, 0x3c, 0xE4, 0x59, 0x64, 0xFF,
			0x21, 0x67, 0xF6, 0xEc, 0xEd, 0xd4, 0x19, 0xdB, 0x06, 0xc1},
	{0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92, 0x81, 0xA1, 0xC7, 0xE2,
				0x84, 0xd7, 0x3E, 0x67, 0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97,
			0x20, 0x0E, 0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0}
};

static int sha256(void) {
	int code = STS_ERR;
	int i, j;
	unsigned char message[MSG_SIZE], digest[32];

	TEST_ONCE("sha256 hash function is correct") {
		for (i = 0; i < TEST_MAX; i++) {
			message[0] = '\0';
			for (j = 0; j < count[i]; j++) {
				strcat((char *)message, tests[i]);
			}
			md_map_sh256(digest, message, strlen((char *)message));
			TEST_ASSERT(memcmp(digest, result256[i], 32) == 0, end);
		}
	}
	TEST_END;

	code = STS_OK;

  end:
	return code;
}

#endif

#if MD_MAP == SH384 || !defined(STRIP)

char *tests2[3] = {
	TEST1,
	TEST4a TEST4b,
	TEST3a TEST3b
};

long int count2[3] = { 1, 1, 10000 };

unsigned char result384[4][48] = {
	{0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B, 0xB5, 0xA0, 0x3D, 0x69,
				0x9A, 0xC6, 0x50, 0x07, 0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE,
				0xD1, 0x63, 0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED,
				0x80, 0x86, 0x07, 0x2B, 0xA1, 0xE7, 0xCC, 0x23, 0x58, 0xBA,
			0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7},
	{0x09, 0x33, 0x0C, 0x33, 0xF7, 0x11, 0x47, 0xE8, 0x3D, 0x19, 0x2F, 0xC7,
				0x82, 0xCD, 0x1B, 0x47, 0x53, 0x11, 0x1B, 0x17, 0x3B, 0x3B,
				0x05, 0xD2, 0x2F, 0xA0, 0x80, 0x86, 0xE3, 0xB0, 0xF7, 0x12,
				0xFC, 0xC7, 0xC7, 0x1A, 0x55, 0x7E, 0x2D, 0xB9, 0x66, 0xC3,
			0xE9, 0xFA, 0x91, 0x74, 0x60, 0x39},
	{0x9D, 0x0E, 0x18, 0x09, 0x71, 0x64, 0x74, 0xCB, 0x08, 0x6E, 0x83, 0x4E,
				0x31, 0x0A, 0x4A, 0x1C, 0xED, 0x14, 0x9E, 0x9C, 0x00, 0xF2,
				0x48, 0x52, 0x79, 0x72, 0xCE, 0xC5, 0x70, 0x4C, 0x2A, 0x5B,
				0x07, 0xB8, 0xB3, 0xDC, 0x38, 0xEC, 0xC4, 0xEB, 0xAE, 0x97,
			0xDD, 0xD8, 0x7F, 0x3D, 0x89, 0x85},
};

static int sha384(void) {
	int code = STS_ERR;
	int i, j;
	unsigned char message[MSG_SIZE], digest[48];

	TEST_ONCE("sha384 hash function is correct") {
		for (i = 0; i < TEST_MAX; i++) {
			message[0] = '\0';
			for (j = 0; j < count2[i]; j++) {
				strcat((char *)message, tests2[i]);
			}
			md_map_sh384(digest, message, strlen((char *)message));
			TEST_ASSERT(memcmp(digest, result384[i], 48) == 0, end);
		}
	}
	TEST_END;

	code = STS_OK;

  end:
	return code;
}

#endif

#if MD_MAP == SH512 || !defined(STRIP)

unsigned char result512[4][64] = {
	{0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA, 0xCC, 0x41, 0x73, 0x49,
				0xAE, 0x20, 0x41, 0x31, 0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9,
				0x7E, 0xA2, 0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A,
				0x21, 0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8, 0x36, 0xBA,
				0x3C, 0x23, 0xA3, 0xFE, 0xEB, 0xBD, 0x45, 0x4D, 0x44, 0x23,
				0x64, 0x3C, 0xE8, 0x0E, 0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C,
			0xA4, 0x9F},
	{0x8E, 0x95, 0x9B, 0x75, 0xDA, 0xE3, 0x13, 0xDA, 0x8C, 0xF4, 0xF7, 0x28,
				0x14, 0xFC, 0x14, 0x3F, 0x8F, 0x77, 0x79, 0xC6, 0xEB, 0x9F,
				0x7F, 0xA1, 0x72, 0x99, 0xAE, 0xAD, 0xB6, 0x88, 0x90, 0x18,
				0x50, 0x1D, 0x28, 0x9E, 0x49, 0x00, 0xF7, 0xE4, 0x33, 0x1B,
				0x99, 0xDE, 0xC4, 0xB5, 0x43, 0x3A, 0xC7, 0xD3, 0x29, 0xEE,
				0xB6, 0xDD, 0x26, 0x54, 0x5E, 0x96, 0xE5, 0x5B, 0x87, 0x4B,
			0xE9, 0x09},
	{0xE7, 0x18, 0x48, 0x3D, 0x0C, 0xE7, 0x69, 0x64, 0x4E, 0x2E, 0x42, 0xC7,
				0xBC, 0x15, 0xB4, 0x63, 0x8E, 0x1F, 0x98, 0xB1, 0x3B, 0x20,
				0x44, 0x28, 0x56, 0x32, 0xA8, 0x03, 0xAF, 0xA9, 0x73, 0xEB,
				0xDE, 0x0F, 0xF2, 0x44, 0x87, 0x7E, 0xA6, 0x0A, 0x4C, 0xB0,
				0x43, 0x2C, 0xE5, 0x77, 0xC3, 0x1B, 0xEB, 0x00, 0x9C, 0x5C,
				0x2C, 0x49, 0xAA, 0x2E, 0x4E, 0xAD, 0xB2, 0x17, 0xAD, 0x8C,
			0xC0, 0x9B},
};

static int sha512(void) {
	int code = STS_ERR;
	int i, j;
	unsigned char message[MSG_SIZE], digest[64];

	TEST_ONCE("sha512 hash function is correct") {
		for (i = 0; i < TEST_MAX; i++) {
			message[0] = '\0';
			for (j = 0; j < count2[i]; j++) {
				strcat((char *)message, tests2[i]);
			}
			md_map_sh512(digest, message, strlen((char *)message));
			TEST_ASSERT(memcmp(digest, result512[i], 64) == 0, end);
		}
	}
	TEST_END;

	code = STS_OK;

  end:
	return code;
}

#endif

unsigned char key1[] = {
	0xB0, 0xAD, 0x56, 0x5B, 0x14, 0xB4, 0x78, 0xCA, 0xD4, 0x76, 0x38, 0x56,
	0xFF, 0x30, 0x16, 0xB1, 0xA9, 0x3D, 0x84, 0x0F, 0x87, 0x26, 0x1B, 0xED,
	0xE7, 0xDD, 0xF0, 0xF9, 0x30, 0x5A, 0x6E, 0x44
};

unsigned char key2[] = {
	0x87, 0x26, 0x1B, 0xED, 0xE7, 0xDD, 0xF0, 0xF9, 0x30, 0x5A, 0x6E, 0x44,
	0xA7, 0x4E, 0x6A, 0x08, 0x46, 0xDE, 0xDE, 0x27, 0xF4, 0x82, 0x05, 0xC6,
	0xB1, 0x41, 0x88, 0x87, 0x42, 0xB0, 0xCE, 0x2C
};

static int kdf(void) {
	int code = STS_ERR;
	unsigned char message[] =
			{ 0XDE, 0XAD, 0xBE, 0xEF, 0xFE, 0xEB, 0xDA, 0xED };
	unsigned char key[32];

#if MD_MAP == SHONE
	TEST_ONCE("kdf1 key derivation function is correct") {
		md_kdf1(key, 32, message, sizeof(message));
		util_print("%d %d\n", key[0], key1[0]);
		TEST_ASSERT(memcmp(key, key1, 32) == 0, end);
	}
	TEST_END;

	TEST_ONCE("kdf2 key derivation function is correct") {
		md_kdf2(key, 32, message, sizeof(message));
		util_print("%d %d\n", key[0], key2[0]);
		TEST_ASSERT(memcmp(key, key2, 32) == 0, end);
	}
	TEST_END;

	code = STS_OK;

  end:
	return code;

#else
	(void) message;
	(void) code;
	(void) key;
	return STS_OK;
#endif
}

static int mgf(void) {
	int code = STS_ERR;
	unsigned char message[] =
			{ 0XDE, 0XAD, 0xBE, 0xEF, 0xFE, 0xEB, 0xDA, 0xED };
	unsigned char key[32];

#if MD_MAP == SHONE
	TEST_ONCE("mgf1 mask generation function is correct") {
		md_mgf1(key, 32, message, sizeof(message));
		TEST_ASSERT(memcmp(key, key1, 32) == 0, end);
	}
	TEST_END;

	code = STS_OK;

  end:
	return code;

#else
	(void) message;
	(void) code;
	(void) key;
	return STS_OK;
#endif
}

int main(void) {
	if (core_init() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Tests for the MD module:\n", 0);

#if MD_MAP == SHONE || !defined(STRIP)
	if (sha1() != STS_OK) {
		core_clean();
		return 1;
	}
#endif

#if MD_MAP == SH224 || !defined(STRIP)
	if (sha224() != STS_OK) {
		core_clean();
		return 1;
	}
#endif

#if MD_MAP == SH256 || !defined(STRIP)
	if (sha256() != STS_OK) {
		core_clean();
		return 1;
	}
#endif

#if MD_MAP == SH384 || !defined(STRIP)
	if (sha384() != STS_OK) {
		core_clean();
		return 1;
	}
#endif

#if MD_MAP == SH512 || !defined(STRIP)
	if (sha512() != STS_OK) {
		core_clean();
		return 1;
	}
#endif

	if (kdf() != STS_OK) {
		core_clean();
		return 1;
	}

	if (mgf() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("All tests have passed.\n", 0);

	core_clean();
	return 0;
}
