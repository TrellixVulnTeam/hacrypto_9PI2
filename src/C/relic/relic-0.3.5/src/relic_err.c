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
 * Implementation of error-handling routines.
 *
 * @version $Id: relic_err.c 1512 2013-08-27 15:42:01Z dfaranha $
 * @ingroup relic
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "relic_core.h"
#include "relic_conf.h"
#include "relic_err.h"

#if defined(VERBS) && OPSYS == LINUX
#include <execinfo.h>
#endif

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

#ifdef CHECK

void err_simple_msg(int error) {
	if (error != ERR_CAUGHT) {
		fprintf(stderr, "\nERROR: %s.\n", core_get()->reason[error]);
	}
}

#ifdef VERBS

void err_full_msg(const char *function, const char *file, int line,
		int error) {
	ctx_t *ctx = core_get();

	if (error == ERR_CAUGHT) {
		fprintf(stderr, "\tCAUGHT in %s() at %s,%d.\n", function, file, line);
	} else {
		fprintf(stderr, "\nERROR in %s() at %s,%d: %s.\n", function, file, line,
				ctx->reason[error]);

#if OPSYS == LINUX
		void *trace[100];
		char **symbols;
		int n;

		/* Print the stack trace. */
		fprintf(stderr, "\tCall stack:\n");
		n = backtrace(trace, 100);
		symbols = backtrace_symbols(trace, n);
		/*
		 * Skip the first entry (err_complete_error) and the two last (this
		 * binary and a libc entry.
		 */
		for (int i = 1; i < n - 2; i++) {
			fprintf(stderr, "\t\t#%d %s\n", i - 1, symbols[i]);
		}
		free(symbols);
#endif
	}
}

#endif /* VERBS */

void err_get_msg(err_t *e, char **msg) {
	ctx_t *ctx = core_get();
	*e = *(ctx->last->error);
	*msg = ctx->reason[*e];
	ctx->last = NULL;
}

#endif /* CHECK */

int err_get_code(void) {
	ctx_t *ctx = core_get();
	int r = ctx->code;
	ctx->code = STS_OK;
	return r;
}
