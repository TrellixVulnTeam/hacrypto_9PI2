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
 * Implementation of the low-level quadratic extension field multiplication
 * functions.
 *
 * @version $Id: relic_pp_sqr_low.c 1521 2013-08-27 15:55:18Z dfaranha $
 * @ingroup pp
 */

#include "relic_fp.h"
#include "relic_pp.h"
#include "relic_core.h"
#include "relic_fp_low.h"
#include "relic_pp_low.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void fp2_sqrn_low(dv2_t c, fp2_t a) {
	align dig_t t0[2 * FP_DIGS], t1[2 * FP_DIGS], t2[2 * FP_DIGS];

	/* t0 = (a0 + a1). */
#ifdef FP_SPACE
	/* if we have room for carries, we can avoid reductions here. */
	fp_addn_low(t0, a[0], a[1]);
#else
	fp_addm_low(t0, a[0], a[1]);
#endif
	/* t1 = (a0 - a1). */
	fp_subm_low(t1, a[0], a[1]);

#ifdef FP_QNRES

#ifdef FP_SPACE
	fp_dbln_low(t2, a[0]);
#else
	fp_dblm_low(t2, a[0]);
#endif
	/* c1 = 2 * a0 * a1. */
	fp_muln_low(c[1], t2, a[1]);
	/* c_0 = a_0^2 + a_1^2 * u^2. */
	fp_muln_low(c[0], t0, t1);

#else /* !FP_QNRES */

	/* t1 = u^2 * (a1 * b1). */
	for (int i = -1; i > fp_prime_get_qnr(); i--) {
		fp_subm_low(t1, t1, a[1]);
	}

	if (fp_prime_get_qnr() == -1) {
		/* t2 = 2 * a0. */
		fp_dbl(t2, a[0]);
		/* c1 = 2 * a0 * a1. */
		fp_muln_low(c[1], t2, a[1]);
		/* c0 = a0^2 + b_0^2 * u^2. */
		fp_muln_low(c[0], t0, t1);
	} else {
		/* c1 = a0 * a1. */
		fp_muln_low(c[1], a[0], a[1]);
		/* c0 = a0^2 + b_0^2 * u^2. */
		fp_muln_low(c[0], t0, t1);

#ifdef FP_SPACE
		for (int i = -1; i > fp_prime_get_qnr(); i--) {
			fp_addd_low(c[0], c[0], c[1]);
		}
		/* c1 = 2 * a0 * a1. */
		fp_addd_low(c[1], c[1], c[1]);
#else
		for (int i = -1; i > fp_prime_get_qnr(); i--) {
			fp_addc_low(c[0], c[0], c[1]);
		}
		/* c1 = 2 * a0 * a1. */
		fp_addc_low(c[1], c[1], c[1]);
#endif
	}
#endif
	/* c = c0 + c1 * u. */
}

void fp2_sqrm_low(fp2_t c, fp2_t a) {
	align dv2_t t;

	dv2_null(t);

	TRY {
		dv2_new(t);
		fp2_sqrn_low(t, a);
		fp2_rdcn_low(c, t);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		dv2_free(t);
	}
}

void fp3_sqrn_low(dv3_t c, fp3_t a) {
	align dig_t t0[2 * FP_DIGS], t1[2 * FP_DIGS], t2[2 * FP_DIGS];
	align dig_t t3[2 * FP_DIGS], t4[2 * FP_DIGS], t5[2 * FP_DIGS];

	/* t0 = a_0^2. */
	fp_sqrn_low(t0, a[0]);

	/* t1 = 2 * a_1 * a_2. */
#ifdef FP_SPACE
	fp_dbln_low(t2, a[1]);
#else
	fp_dblm_low(t2, a[1]);
#endif

	fp_muln_low(t1, t2, a[2]);

	/* t2 = a_2^2. */
	fp_sqrn_low(t2, a[2]);

	/* t3 = (a_0 + a_2 + a_1)^2, t4 = (a_0 + a_2 - a_1)^2. */
#ifdef FP_SPACE
	fp_addn_low(t3, a[0], a[2]);
	fp_addn_low(t4, t3, a[1]);
#else
	fp_addm_low(t3, a[0], a[2]);
	fp_addm_low(t4, t3, a[1]);
#endif
	fp_subm_low(t5, t3, a[1]);
	fp_sqrn_low(t3, t4);
	fp_sqrn_low(t4, t5);

	/* t4 = (t4 + t3)/2. */
	fp_addd_low(t4, t4, t3);
	fp_hlvd_low(t4, t4);

	/* t3 = t3 - t4 - t1. */
	fp_addc_low(t5, t1, t4);
	fp_subc_low(t3, t3, t5);

	/* c_2 = t4 - t0 - t2. */
	fp_addc_low(t5, t0, t2);
	fp_subc_low(c[2], t4, t5);

	/* c_0 = t0 + t1 * B. */
	fp_subc_low(c[0], t0, t1);
	for (int i = -1; i > fp_prime_get_cnr(); i--) {
		fp_subc_low(c[0], c[0], t1);
	}

	/* c_1 = t3 + t2 * B. */
	fp_subc_low(c[1], t3, t2);
	for (int i = -1; i > fp_prime_get_cnr(); i--) {
		fp_subc_low(c[1], c[1], t2);
	}
}

void fp3_sqrm_low(fp3_t c, fp3_t a) {
	align dv3_t t;

	dv3_null(t);

	TRY {
		dv3_new(t);
		fp3_sqrn_low(t, a);
		fp3_rdcn_low(c, t);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		dv3_free(t);
	}
}
