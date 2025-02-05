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
 * Tests for the Pairing-Based Cryptography module.
 *
 * @version $Id: test_pc.c 1579 2013-09-03 06:12:28Z dfaranha $
 * @ingroup test
 */

#include <stdio.h>

#include "relic.h"
#include "relic_test.h"

static int memory1(void) {
	err_t e;
	int code = STS_ERR;
	g1_t a;

	g1_null(a);

	TRY {
		TEST_BEGIN("memory can be allocated") {
			g1_new(a);
			g1_free(a);
		} TEST_END;
	} CATCH(e) {
		switch (e) {
			case ERR_NO_MEMORY:
				util_print("FATAL ERROR!\n");
				ERROR(end);
				break;
		}
	}
	(void)a;
	code = STS_OK;
  end:
	return code;
}

int util1(void) {
	int code = STS_ERR;
	g1_t a, b, c;

	g1_null(a);
	g1_null(b);
	g1_null(c);

	TRY {
		g1_new(a);
		g1_new(b);
		g1_new(c);

		TEST_BEGIN("comparison is consistent") {
			g1_rand(a);
			g1_rand(b);
			TEST_ASSERT(g1_cmp(a, b) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("copy and comparison are consistent") {
			g1_rand(a);
			g1_rand(b);
			g1_rand(c);
			if (g1_cmp(a, c) != CMP_EQ) {
				g1_copy(c, a);
				TEST_ASSERT(g1_cmp(c, a) == CMP_EQ, end);
			}
			if (g1_cmp(b, c) != CMP_EQ) {
				g1_copy(c, b);
				TEST_ASSERT(g1_cmp(b, c) == CMP_EQ, end);
			}
		}
		TEST_END;

		TEST_BEGIN("inversion and comparison are consistent") {
			g1_rand(a);
			g1_neg(b, a);
			TEST_ASSERT(g1_cmp(a, b) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN
				("assignment to random/infinity and comparison are consistent")
		{
			g1_rand(a);
			g1_set_infty(c);
			TEST_ASSERT(g1_cmp(a, c) != CMP_EQ, end);
			TEST_ASSERT(g1_cmp(c, a) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to infinity and infinity test are consistent") {
			g1_set_infty(a);
			TEST_ASSERT(g1_is_infty(a), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	g1_free(a);
	g1_free(b);
	g1_free(c);
	return code;
}

int addition1(void) {
	int code = STS_ERR;

	g1_t a, b, c, d, e;

	g1_null(a);
	g1_null(b);
	g1_null(c);
	g1_null(d);
	g1_null(e);

	TRY {
		g1_new(a);
		g1_new(b);
		g1_new(c);
		g1_new(d);
		g1_new(e);

		TEST_BEGIN("point addition is commutative") {
			g1_rand(a);
			g1_rand(b);
			g1_add(d, a, b);
			g1_add(e, b, a);
			g1_norm(d, d);
			g1_norm(e, e);
			TEST_ASSERT(g1_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition is associative") {
			g1_rand(a);
			g1_rand(b);
			g1_rand(c);
			g1_add(d, a, b);
			g1_add(d, d, c);
			g1_add(e, b, c);
			g1_add(e, e, a);
			g1_norm(d, d);
			g1_norm(e, e);
			TEST_ASSERT(g1_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition has identity") {
			g1_rand(a);
			g1_set_infty(d);
			g1_add(e, a, d);
			TEST_ASSERT(g1_cmp(e, a) == CMP_EQ, end);
			g1_add(e, d, a);
			TEST_ASSERT(g1_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition has inverse") {
			g1_rand(a);
			g1_neg(d, a);
			g1_add(e, a, d);
			TEST_ASSERT(g1_is_infty(e), end);
		} TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	g1_free(a);
	g1_free(b);
	g1_free(c);
	g1_free(d);
	g1_free(e);
	return code;
}

int subtraction1(void) {
	int code = STS_ERR;
	g1_t a, b, c, d;

	g1_null(a);
	g1_null(b);
	g1_null(c);
	g1_null(d);

	TRY {
		g1_new(a);
		g1_new(b);
		g1_new(c);
		g1_new(d);

		TEST_BEGIN("point subtraction is anti-commutative") {
			g1_rand(a);
			g1_rand(b);
			g1_sub(c, a, b);
			g1_sub(d, b, a);
			g1_norm(c, c);
			g1_norm(d, d);
			g1_neg(d, d);
			TEST_ASSERT(g1_cmp(c, d) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("point subtraction has identity") {
			g1_rand(a);
			g1_set_infty(c);
			g1_sub(d, a, c);
			g1_norm(d, d);
			TEST_ASSERT(g1_cmp(d, a) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("point subtraction has inverse") {
			g1_rand(a);
			g1_sub(c, a, a);
			g1_norm(c, c);
			TEST_ASSERT(g1_is_infty(c), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	g1_free(a);
	g1_free(b);
	g1_free(c);
	g1_free(d);
	return code;
}

int doubling1(void) {
	int code = STS_ERR;
	g1_t a, b, c;

	g1_null(a);
	g1_null(b);
	g1_null(c);

	TRY {
		g1_new(a);
		g1_new(b);
		g1_new(c);

		TEST_BEGIN("point doubling is correct") {
			g1_rand(a);
			g1_add(b, a, a);
			g1_norm(b, b);
			g1_dbl(c, a);
			g1_norm(c, c);
			TEST_ASSERT(g1_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	g1_free(a);
	g1_free(b);
	g1_free(c);
	return code;
}

static int multiplication1(void) {
	int code = STS_ERR;
	g1_t p, q, r;
	bn_t n, k;

	bn_null(n);
	bn_null(k);
	g1_null(p);
	g1_null(q);
	g1_null(r);

	TRY {
		g1_new(p);
		g1_new(q);
		g1_new(r);
		bn_new(n);
		bn_new(k);

		g1_get_gen(p);
		g1_get_ord(n);

		TEST_BEGIN("generator has the right order") {
			g1_mul(r, p, n);
			TEST_ASSERT(g1_is_infty(r) == 1, end);
		} TEST_END;

		TEST_BEGIN("generator multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			g1_mul(q, p, k);
			g1_mul_gen(r, k);
			TEST_ASSERT(g1_cmp(q, r) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("random element has the right order") {
			g1_rand(p);
			g1_mul(r, p, n);
			TEST_ASSERT(g1_is_infty(r) == 1, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	g1_free(p);
	g1_free(q);
	g1_free(r);
	bn_free(n);
	bn_free(k);
	return code;
}

static int fixed1(void) {
	int code = STS_ERR;
	g1_t p, q, r;
	g1_t t[G1_TABLE];
	bn_t n, k;

	bn_null(n);
	bn_null(k);
	g1_null(p);
	g1_null(q);
	g1_null(r);

	for (int i = 0; i < G1_TABLE; i++) {
		g1_null(t[i]);
	}

	TRY {
		g1_new(p);
		g1_new(q);
		g1_new(r);
		bn_new(n);
		bn_new(k);

		g1_get_gen(p);
		g1_get_ord(n);

		for (int i = 0; i < G1_TABLE; i++) {
			g1_new(t[i]);
		}
		TEST_BEGIN("fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			g1_mul(q, p, k);
			g1_mul_pre(t, p);
			g1_mul_fix(q, (const g1_t *)t, k);
			g1_mul(r, p, k);
			TEST_ASSERT(g1_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < G1_TABLE; i++) {
			g1_free(t[i]);
		}
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	g1_free(p);
	g1_free(q);
	g1_free(r);
	bn_free(n);
	bn_free(k);
	return code;
}

static int simultaneous1(void) {
	int code = STS_ERR;
	g1_t p, q, r, s;
	bn_t n, k, l;

	bn_null(n);
	bn_null(k);
	bn_null(l);
	g1_null(p);
	g1_null(q);
	g1_null(r);
	g1_null(s);

	TRY {
		bn_new(n);
		bn_new(k);
		bn_new(l);
		g1_new(p);
		g1_new(q);
		g1_new(r);
		g1_new(s);

		g1_get_gen(p);
		g1_get_ord(n);

		TEST_BEGIN("simultaneous point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			g1_mul(q, p, k);
			g1_mul(s, q, l);
			g1_mul_sim(r, p, k, q, l);
			g1_add(q, q, s);
			g1_norm(q, q);
			TEST_ASSERT(g1_cmp(q, r) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("simultaneous multiplication with generator is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			g1_mul_sim(r, p, k, q, l);
			g1_get_gen(s);
			g1_mul_sim(q, s, k, q, l);
			TEST_ASSERT(g1_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	bn_free(n);
	bn_free(k);
	bn_free(l);
	g1_free(p);
	g1_free(q);
	g1_free(r);
	g1_free(s);
	return code;
}

static int hashing1(void) {
	int code = STS_ERR;
	g1_t a;
	bn_t n;
	unsigned char msg[5];

	g1_null(a);
	bn_null(n);

	TRY {
		g1_new(a);
		bn_new(n);

		g1_get_ord(n);

		TEST_BEGIN("point hashing is correct") {
			rand_bytes(msg, sizeof(msg));
			g1_map(a, msg, sizeof(msg));
			g1_mul(a, a, n);
			TEST_ASSERT(g1_is_infty(a) == 1, end);
		}
		TEST_END;

	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	g1_free(a);
	bn_free(n);
	return code;
}

static int memory2(void) {
	err_t e;
	int code = STS_ERR;
	g2_t a;

	g2_null(a);

	TRY {
		TEST_BEGIN("memory can be allocated") {
			g2_new(a);
			g2_free(a);
		} TEST_END;
	} CATCH(e) {
		switch (e) {
			case ERR_NO_MEMORY:
				util_print("FATAL ERROR!\n");
				ERROR(end);
				break;
		}
	}
	(void)a;
	code = STS_OK;
  end:
	return code;
}

int util2(void) {
	int code = STS_ERR;
	g2_t a, b, c;

	g2_null(a);
	g2_null(b);
	g2_null(c);

	TRY {
		g2_new(a);
		g2_new(b);
		g2_new(c);

		TEST_BEGIN("comparison is consistent") {
			g2_rand(a);
			g2_rand(b);
			TEST_ASSERT(g2_cmp(a, b) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("copy and comparison are consistent") {
			g2_rand(a);
			g2_rand(b);
			g2_rand(c);
			if (g2_cmp(a, c) != CMP_EQ) {
				g2_copy(c, a);
				TEST_ASSERT(g2_cmp(c, a) == CMP_EQ, end);
			}
			if (g2_cmp(b, c) != CMP_EQ) {
				g2_copy(c, b);
				TEST_ASSERT(g2_cmp(b, c) == CMP_EQ, end);
			}
		}
		TEST_END;

		TEST_BEGIN("negation and comparison are consistent") {
			g2_rand(a);
			g2_neg(b, a);
			TEST_ASSERT(g2_cmp(a, b) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN
				("assignment to random/infinity and comparison are consistent")
		{
			g2_rand(a);
			g2_set_infty(c);
			TEST_ASSERT(g2_cmp(a, c) != CMP_EQ, end);
			TEST_ASSERT(g2_cmp(c, a) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to infinity and infinity test are consistent") {
			g2_set_infty(a);
			TEST_ASSERT(g2_is_infty(a), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	g2_free(a);
	g2_free(b);
	g2_free(c);
	return code;
}

int addition2(void) {
	int code = STS_ERR;

	g2_t a, b, c, d, e;

	g2_null(a);
	g2_null(b);
	g2_null(c);
	g2_null(d);
	g2_null(e);

	TRY {
		g2_new(a);
		g2_new(b);
		g2_new(c);
		g2_new(d);
		g2_new(e);

		TEST_BEGIN("point addition is commutative") {
			g2_rand(a);
			g2_rand(b);
			g2_add(d, a, b);
			g2_add(e, b, a);
			g2_norm(d, d);
			g2_norm(e, e);
			TEST_ASSERT(g2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition is associative") {
			g2_rand(a);
			g2_rand(b);
			g2_rand(c);
			g2_add(d, a, b);
			g2_add(d, d, c);
			g2_add(e, b, c);
			g2_add(e, e, a);
			g2_norm(d, d);
			g2_norm(e, e);
			TEST_ASSERT(g2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition has identity") {
			g2_rand(a);
			g2_set_infty(d);
			g2_add(e, a, d);
			TEST_ASSERT(g2_cmp(e, a) == CMP_EQ, end);
			g2_add(e, d, a);
			TEST_ASSERT(g2_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition has inverse") {
			g2_rand(a);
			g2_neg(d, a);
			g2_add(e, a, d);
			TEST_ASSERT(g2_is_infty(e), end);
		} TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	g2_free(a);
	g2_free(b);
	g2_free(c);
	g2_free(d);
	g2_free(e);
	return code;
}

int subtraction2(void) {
	int code = STS_ERR;
	g2_t a, b, c, d;

	g2_null(a);
	g2_null(b);
	g2_null(c);
	g2_null(d);

	TRY {
		g2_new(a);
		g2_new(b);
		g2_new(c);
		g2_new(d);

		TEST_BEGIN("point subtraction is anti-commutative") {
			g2_rand(a);
			g2_rand(b);
			g2_sub(c, a, b);
			g2_sub(d, b, a);
			g2_norm(c, c);
			g2_norm(d, d);
			g2_neg(d, d);
			TEST_ASSERT(g2_cmp(c, d) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("point subtraction has identity") {
			g2_rand(a);
			g2_set_infty(c);
			g2_sub(d, a, c);
			g2_norm(d, d);
			TEST_ASSERT(g2_cmp(d, a) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("point subtraction has inverse") {
			g2_rand(a);
			g2_sub(c, a, a);
			g2_norm(c, c);
			TEST_ASSERT(g2_is_infty(c), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	g2_free(a);
	g2_free(b);
	g2_free(c);
	g2_free(d);
	return code;
}

int doubling2(void) {
	int code = STS_ERR;
	g2_t a, b, c;

	g2_null(a);
	g2_null(b);
	g2_null(c);

	TRY {
		g2_new(a);
		g2_new(b);
		g2_new(c);

		TEST_BEGIN("point doubling is correct") {
			g2_rand(a);
			g2_add(b, a, a);
			g2_norm(b, b);
			g2_dbl(c, a);
			g2_norm(c, c);
			TEST_ASSERT(g2_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	g2_free(a);
	g2_free(b);
	g2_free(c);
	return code;
}

static int multiplication2(void) {
	int code = STS_ERR;
	g2_t p, q, r;
	bn_t n, k;

	bn_null(n);
	bn_null(k);
	g2_null(p);
	g2_null(q);
	g2_null(r);

	TRY {
		g2_new(p);
		g2_new(q);
		g2_new(r);
		bn_new(n);
		bn_new(k);

		g2_get_gen(p);
		g2_get_ord(n);

		TEST_BEGIN("generator has the right order") {
			g2_mul(r, p, n);
			TEST_ASSERT(g2_is_infty(r) == 1, end);
		} TEST_END;

		TEST_BEGIN("generator multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			g2_mul(q, p, k);
			g2_mul_gen(r, k);
			TEST_ASSERT(g2_cmp(q, r) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("random element has the right order") {
			g2_rand(p);
			g2_mul(r, p, n);
			TEST_ASSERT(g2_is_infty(r) == 1, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	g2_free(p);
	g2_free(q);
	g2_free(r);
	bn_free(n);
	bn_free(k);
	return code;
}

static int fixed2(void) {
	int code = STS_ERR;
	g2_t p, q, r;
	g2_t t[G2_TABLE];
	bn_t n, k;

	bn_null(n);
	bn_null(k);
	g2_null(p);
	g2_null(q);
	g2_null(r);

	for (int i = 0; i < G2_TABLE; i++) {
		g2_null(t[i]);
	}

	TRY {
		g2_new(p);
		g2_new(q);
		g2_new(r);
		bn_new(n);
		bn_new(k);

		g2_get_gen(p);
		g2_get_ord(n);

		for (int i = 0; i < G2_TABLE; i++) {
			g2_new(t[i]);
		}
		TEST_BEGIN("fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			g2_mul(q, p, k);
			g2_mul_pre(t, p);
			g2_mul_fix(q, t, k);
			g2_mul(r, p, k);
			TEST_ASSERT(g2_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < G2_TABLE; i++) {
			g2_free(t[i]);
		}
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	g2_free(p);
	g2_free(q);
	g2_free(r);
	bn_free(n);
	bn_free(k);
	return code;
}

static int simultaneous2(void) {
	int code = STS_ERR;
	g2_t p, q, r, s;
	bn_t n, k, l;

	bn_null(n);
	bn_null(k);
	bn_null(l);
	g2_null(p);
	g2_null(q);
	g2_null(r);
	g2_null(s);

	TRY {
		bn_new(n);
		bn_new(k);
		bn_new(l);
		g2_new(p);
		g2_new(q);
		g2_new(r);
		g2_new(s);

		g2_get_gen(p);
		g2_get_ord(n);

		TEST_BEGIN("simultaneous point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			g2_mul(q, p, k);
			g2_mul(s, q, l);
			g2_mul_sim(r, p, k, q, l);
			g2_add(q, q, s);
			g2_norm(q, q);
			TEST_ASSERT(g2_cmp(q, r) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("simultaneous multiplication with generator is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			g2_mul_sim_gen(r, k, q, l);
			g2_get_gen(s);
			g2_mul_sim(q, s, k, q, l);
			TEST_ASSERT(g2_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	bn_free(n);
	bn_free(k);
	bn_free(l);
	g2_free(p);
	g2_free(q);
	g2_free(r);
	g2_free(s);
	return code;
}

static int hashing2(void) {
	int code = STS_ERR;
	g2_t a;
	bn_t n;
	unsigned char msg[5];

	g2_null(a);
	bn_null(n);

	TRY {
		g2_new(a);
		bn_new(n);

		g2_get_ord(n);

		TEST_BEGIN("point hashing is correct") {
			rand_bytes(msg, sizeof(msg));
			g2_map(a, msg, sizeof(msg));
			g2_mul(a, a, n);
			TEST_ASSERT(g2_is_infty(a) == 1, end);
		}
		TEST_END;

	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	g2_free(a);
	bn_free(n);
	return code;
}

static int memory(void) {
	err_t e;
	int code = STS_ERR;
	gt_t a;

	gt_null(a);

	TRY {
		TEST_BEGIN("memory can be allocated") {
			gt_new(a);
			gt_free(a);
		} TEST_END;
	} CATCH(e) {
		switch (e) {
			case ERR_NO_MEMORY:
				util_print("FATAL ERROR!\n");
				ERROR(end);
				break;
		}
	}
	(void)a;
	code = STS_OK;
  end:
	return code;
}

int util(void) {
	int code = STS_ERR;
	gt_t a, b, c;

	gt_null(a);
	gt_null(b);
	gt_null(c);

	TRY {
		gt_new(a);
		gt_new(b);
		gt_new(c);

		TEST_BEGIN("comparison is consistent") {
			gt_rand(a);
			gt_rand(b);
			TEST_ASSERT(gt_cmp(a, b) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("copy and comparison are consistent") {
			gt_rand(a);
			gt_rand(b);
			gt_rand(c);
			if (gt_cmp(a, c) != CMP_EQ) {
				gt_copy(c, a);
				TEST_ASSERT(gt_cmp(c, a) == CMP_EQ, end);
			}
			if (gt_cmp(b, c) != CMP_EQ) {
				gt_copy(c, b);
				TEST_ASSERT(gt_cmp(b, c) == CMP_EQ, end);
			}
		}
		TEST_END;

		TEST_BEGIN("inversion and comparison are consistent") {
			gt_rand(a);
			gt_inv(b, a);
			TEST_ASSERT(gt_cmp(a, b) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN
				("assignment to random/infinity and comparison are consistent")
		{
			gt_rand(a);
			gt_set_unity(c);
			TEST_ASSERT(gt_cmp(a, c) != CMP_EQ, end);
			TEST_ASSERT(gt_cmp(c, a) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to infinity and infinity test are consistent") {
			gt_set_unity(a);
			TEST_ASSERT(gt_is_unity(a), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	gt_free(a);
	gt_free(b);
	gt_free(c);
	return code;
}

int multiplication(void) {
	int code = STS_ERR;

	gt_t a, b, c, d, e;

	gt_null(a);
	gt_null(b);
	gt_null(c);
	gt_null(d);
	gt_null(e);

	TRY {
		gt_new(a);
		gt_new(b);
		gt_new(c);
		gt_new(d);
		gt_new(e);

		TEST_BEGIN("multiplication is commutative") {
			gt_rand(a);
			gt_rand(b);
			gt_mul(d, a, b);
			gt_mul(e, b, a);
			TEST_ASSERT(gt_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication is associative") {
			gt_rand(a);
			gt_rand(b);
			gt_rand(c);
			gt_mul(d, a, b);
			gt_mul(d, d, c);
			gt_mul(e, b, c);
			gt_mul(e, e, a);
			TEST_ASSERT(gt_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication has identity") {
			gt_rand(a);
			gt_set_unity(d);
			gt_mul(e, a, d);
			TEST_ASSERT(gt_cmp(e, a) == CMP_EQ, end);
			gt_mul(e, d, a);
			TEST_ASSERT(gt_cmp(e, a) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	gt_free(a);
	gt_free(b);
	gt_free(c);
	gt_free(d);
	gt_free(e);
	return code;
}

int squaring(void) {
	int code = STS_ERR;
	gt_t a, b, c;

	gt_null(a);
	gt_null(b);
	gt_null(c);

	TRY {
		gt_new(a);
		gt_new(b);
		gt_new(c);

		TEST_BEGIN("squaring is correct") {
			gt_rand(a);
			gt_mul(b, a, a);
			gt_sqr(c, a);
			TEST_ASSERT(gt_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	gt_free(a);
	gt_free(b);
	gt_free(c);
	return code;
}

static int inversion(void) {
	int code = STS_ERR;
	gt_t a, b, c;

	TRY {
		gt_new(a);
		gt_new(b);
		gt_new(c);

		TEST_BEGIN("inversion is correct") {
			gt_rand(a);
			gt_inv(b, a);
			gt_mul(c, a, b);
			gt_set_unity(b);
			TEST_ASSERT(gt_cmp(c, b) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	gt_free(a);
	gt_free(b);
	gt_free(c);
	return code;
}

int exponentiation(void) {
	int code = STS_ERR;
	gt_t a, c;
	bn_t n;

	gt_null(a);
	gt_null(c);
	bn_null(n);

	TRY {
		gt_new(a);
		gt_new(c);
		bn_new(n);

		gt_get_gen(a);
		gt_get_ord(n);

		TEST_BEGIN("generator has the right order") {
			gt_exp(c, a, n);
			TEST_ASSERT(gt_is_unity(c), end);
		} TEST_END;

		TEST_BEGIN("random element has the correct order") {
			gt_rand(a);
			gt_exp(c, a, n);
			TEST_ASSERT(gt_is_unity(c), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	gt_free(a);
	gt_free(c);
	bn_free(n);
	return code;
}

static int pairing(void) {
	int code = STS_ERR;
	gt_t e1, e2;
	g1_t p, q;
	g2_t r, s;
	bn_t k, n;

	gt_null(e1);
	gt_null(e2);
	g1_null(p);
	g1_null(q);
	g2_null(r);
	g2_null(s);
	bn_null(k);
	bn_null(n);

	TRY {
		gt_new(e1);
		gt_new(e2);
		g1_new(p);
		g1_new(q);
		g2_new(r);
		g2_new(s);
		bn_new(k);
		bn_new(n);

		g1_get_ord(n);

		TEST_BEGIN("pairing is not degenerate") {
			g1_rand(p);
			g2_rand(r);
			pc_map(e1, p, r);
			gt_set_unity(e2);
			TEST_ASSERT(gt_cmp(e1, e2) != CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("pairing is bilinear") {
			g1_rand(p);
			g2_rand(r);
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			g1_mul(q, p, k);
			pc_map(e1, q, r);
			g2_mul(s, r, k);
			pc_map(e2, p, s);
			TEST_ASSERT(gt_cmp(e1, e2) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	gt_free(e1);
	gt_free(e2);
	g1_free(p);
	g1_free(q);
	g2_free(r);
	g2_free(s);
	bn_free(k);
	bn_free(n);
	return code;
}

int test1(void) {
	util_banner("Utilities:", 1);

	if (memory1() != STS_OK) {
		core_clean();
		return 1;
	}

	if (util1() != STS_OK) {
		return STS_ERR;
	}

	util_banner("Arithmetic:", 1);

	if (addition1() != STS_OK) {
		return STS_ERR;
	}

	if (subtraction1() != STS_OK) {
		return STS_ERR;
	}

	if (doubling1() != STS_OK) {
		return STS_ERR;
	}

	if (multiplication1() != STS_OK) {
		return STS_ERR;
	}

	if (fixed1() != STS_OK) {
		return STS_ERR;
	}

	if (simultaneous1() != STS_OK) {
		return STS_ERR;
	}

	if (hashing1() != STS_OK) {
		return STS_ERR;
	}

	return STS_OK;
}

int test2(void) {
	util_banner("Utilities:", 1);

	if (memory2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (util2() != STS_OK) {
		return STS_ERR;
	}

	util_banner("Arithmetic:", 1);

	if (addition2() != STS_OK) {
		return STS_ERR;
	}

	if (subtraction2() != STS_OK) {
		return STS_ERR;
	}

	if (doubling2() != STS_OK) {
		return STS_ERR;
	}

	if (multiplication2() != STS_OK) {
		return STS_ERR;
	}

	if (fixed2() != STS_OK) {
		return STS_ERR;
	}

	if (simultaneous2() != STS_OK) {
		return STS_ERR;
	}

	if (hashing2() != STS_OK) {
		return STS_ERR;
	}

	return STS_OK;
}

int test(void) {
	util_banner("Utilities:", 1);

	if (memory() != STS_OK) {
		core_clean();
		return 1;
	}

	if (util() != STS_OK) {
		return STS_ERR;
	}

	util_banner("Arithmetic:", 1);

	if (multiplication() != STS_OK) {
		return STS_ERR;
	}

	if (squaring() != STS_OK) {
		return STS_ERR;
	}

	if (inversion() != STS_OK) {
		return STS_ERR;
	}

	if (exponentiation() != STS_OK) {
		return STS_ERR;
	}

	if (pairing() != STS_OK) {
		return STS_ERR;
	}

	return STS_OK;
}

int main(void) {
	if (core_init() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Tests for the PC module:", 0);

	if (pc_param_set_any() != STS_OK) {
		THROW(ERR_NO_CURVE);
		core_clean();
		return 0;
	}

	pc_param_print();

	util_banner("Group G_1:", 0);
	if (test1() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Group G_2:", 0);
	if (test2() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Group G_T:", 0);
	if (test() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("All tests have passed.\n", 0);

	core_clean();
	return 0;
}
