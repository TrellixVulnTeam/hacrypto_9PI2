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
 * Interface of the error-handling functions.
 *
 * @version $Id: relic_err.h 1523 2013-08-27 16:12:11Z dfaranha $
 * @ingroup relic
 */

#ifndef RELIC_ERROR_H
#define RELIC_ERROR_H

#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "relic_core.h"
#include "relic_conf.h"
#include "relic_util.h"
#include "relic_label.h"

/*============================================================================*/
/* Constant definitions                                                       */
/*============================================================================*/

/**
 * List of possible errors generated by the library.
 */
enum errors {
	/** Constant to indicate an error already catched. */
	ERR_CAUGHT = 0,
	/** Constant to indicate the first error code. */
	ERR_FIRST = 1,
	/** Occurs when memory-allocating functions fail. */
	ERR_NO_MEMORY = 1,
	/** Occcurs when the library precision is not sufficient. */
	ERR_NO_PRECI,
	/** Occurs when a file is not found. */
	ERR_NO_FILE,
	/** Occurs when the specified number of bytes cannot be read from source. */
	ERR_NO_READ,
	/** Occurs when an invalid value is passed as input. */
	ERR_NO_VALID,
	/** Occurs when a buffer capacity is insufficient. */
	ERR_NO_BUFFER,
	/** Occurs when there is not a supported field in the security level. */
	ERR_NO_FIELD,
	/** Occurs when there is not a supported curve in the security level. */
	ERR_NO_CURVE,
	/** Occurs when the library configuration is incorrect. */
	ERR_NO_CONFIG,
	/** Constant to indicate the number of errors. */
	ERR_LAST
};

/** Constant to indicate the number of possible errors. */
#define ERR_MAX			(ERR_LAST - ERR_FIRST + 1)

/** Error message respective to ERR_NO_MEMORY. */
#define MSG_NO_MEMORY 		"not enough memory"
/** Error message respective to ERR_PRECISION. */
#define MSG_NO_PRECI 		"insufficient precision"
/** Error message respective to ERR_NO FILE. */
#define MSG_NO_FILE			"file not found"
/** Error message respective to ERR_NO_READ. */
#define MSG_NO_READ			"can't read bytes from file"
/** Error message respective to ERR_NO_VALID. */
#define MSG_NO_VALID		"invalid value passed as input"
/** Error message respective to ERR_NO_BUFFER. */
#define MSG_NO_BUFFER		"insufficient buffer capacity"
/** Error message respective to ERR_NO_FIELD. */
#define MSG_NO_FIELD		"no finite field supported at this security level"
/** Error message respective to ERR_NO_CURVE. */
#define MSG_NO_CURVE		"no curve supported at this security level"
/** Error message respective to ERR_NO_CONFIG. */
#define MSG_NO_CONFIG		"invalid library configuration"

/** Truncate file name if verbosity is turned off. */
#ifdef VERBS
#define THIS_FILE			__FILE__
#else
#define THIS_FILE			((strrchr(__FILE__, '/') ? : __FILE__ - 1) + 1)
#endif

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/**
 * Type that represents an error.
 */
typedef int err_t;

/**
 * Type that describes an error status, including the error code and the program
 * location where the error occurred.
 */
typedef struct _sts_t {
	/** Error occurred. */
	err_t *error;
	/** Pointer to the program location where the error occurred. */
	jmp_buf addr;
	/** Flag to tell if there is a surrounding try-catch block. */
	int block;
} sts_t;

/*============================================================================*/
/* Macro definitions                                                          */
/*============================================================================*/

/**
 * Implements the TRY clause of the error-handling routines.
 *
 * This macro copies the last error from the current library context to
 * a temporary variable and handles the current error. The loop is used so
 * the CATCH facility is called first to store the address of the error
 * being caught. The setjmp() function is then called to store the current
 * program location in the current error field. The program block can now be
 * executed. If an error is thrown inside the program block, the setjmp()
 * function is called again and the return value is non-zero.
 */
#define ERR_TRY															\
	{																	\
		sts_t *_last, _this;											\
		ctx_t *_ctx = core_get();										\
		_last = _ctx->last; 											\
		_this.block = 1;												\
		_ctx->last = &_this; 											\
		for (int _z = 0; ; _z = 1) 										\
			if (_z) { 													\
				if (setjmp(_this.addr) == 0) { 							\
					if (1)												\

/**
 * Implements the CATCH clause of the error-handling routines.
 *
 * First, the address of the error is stored and the execution resumes
 * on the ERR_TRY macro. If an error is thrown inside the program block,
 * the caught flag is updated and the last error is restored. If some error
 * was caught, the execution is resumed inside the CATCH block.
 *
 * @param[in] ADDR	- the address of the exception being caught
 */
#define ERR_CATCH(ADDR)													\
					else { } 											\
					_ctx->caught = 0; 									\
				} else {												\
					_ctx->caught = 1; 									\
				}														\
				_ctx->last = _last;										\
				break; 													\
			} else {													\
				_this.error = ADDR; 									\
			}															\
	} 																	\
	for (int _z = 0; _z < 2; _z++) 										\
		if (_z == 1 && core_get()->caught) 								\

/**
 * Implements the THROW clause of the error-handling routines.
 *
 * If the error pointer is not NULL but there is no surrounding TRY-CATCH
 * block, then the code threw an exception after an exception was thrown.
 * In this case, we finish execution.
 *
 * If the error pointer is NULL, the error was thrown outside of a TRY-CATCH
 * block. An error message is printed and the function returns.
 *
 * If the error pointer is valid, the longjmp() function is called to return to
 * the program location where setjmp() was last called. An error message
 * respective to the error is then printed and the current error pointer is
 * updated to store the error.
 *
 * @param[in] E		- the exception being caught.
 */
#define ERR_THROW(E)													\
	{																	\
		ctx_t *_ctx = core_get();										\
		_ctx->code = STS_ERR;											\
		if (_ctx->last != NULL && _ctx->last->block == 0) {				\
			exit(E);													\
		}																\
		if (_ctx->last == NULL) {										\
			_ctx->last = &(_ctx->error);								\
			_ctx->error.error = &(_ctx->number);						\
			_ctx->error.block = 0;										\
			_ctx->number = E;											\
			ERR_PRINT(E);												\
		} else {														\
			for (; ; longjmp(_ctx->last->addr, 1)) {					\
				ERR_PRINT(E);											\
				if (_ctx->last->error) {								\
					if (E != ERR_CAUGHT) {								\
						*(_ctx->last->error) = E;						\
					}													\
				}														\
			}															\
		}																\
	}																	\

#ifdef CHECK
/**
 * Implements a TRY clause.
 */
#define TRY					ERR_TRY
#else
/**
 * Stub for the TRY clause.
 */
#define TRY					if (1)
#endif

#ifdef CHECK
/**
 * Implements a CATCH clause.
 */
#define CATCH(E)			ERR_CATCH(&(E))
#else
/**
 * Stub for the CATCH clause.
 */
#define CATCH(E)			else
#endif

#ifdef CHECK
/**
 * Implements a CATCH clause for any possible error.
 *
 * If this macro is used the error type is not available inside the CATCH
 * block.
 */
#define CATCH_ANY			ERR_CATCH(NULL)
#else
/**
 * Stub for the CATCH_ANY clause.
 */
#define CATCH_ANY			if (0)
#endif

#ifdef CHECK
/**
 * Implements a FINALLY clause.
 */
#define FINALLY				else if (_z == 0)
#else
#define FINALLY				if (1)
#endif

#ifdef CHECK
/**
 * Implements a THROW clause.
 */
#define THROW				ERR_THROW
#else
/**
 * Stub for the THROW clause.
 */
#ifdef QUIET
#define THROW(E)			core_get()->code = STS_ERR;
#else
#define THROW(E)															\
	core_get()->code = STS_ERR; 											\
	util_print("FATAL ERROR in %s:%d\n", THIS_FILE, __LINE__);				\

#endif
#endif

/**
 * Treats an error jumping to the argument.
 *
 * @param[in] LABEL			- the label to jump
 */
#define ERROR(LABEL)		goto LABEL

#ifdef VERBS

/**
 * Prints the current error message.
 *
 * @param[in] ERROR			- the error code.
 */
#define ERR_PRINT(ERROR)													\
	err_full_msg(__func__, __FILE__, __LINE__, ERROR)						\

#else

/**
 * Prints the current error message.
 *
 * @param[in] ERROR			- the error code.
 */
#define ERR_PRINT(ERROR)													\
	err_simple_msg(ERROR)													\

#endif

/*============================================================================*/
/* Function prototypes                                                        */
/*============================================================================*/

#ifdef CHECK

/**
 * Prints the error message with little information.
 *
 * @param[in] error    		- the error code.
 */
void err_simple_msg(int error);

/**
 * Prints the error message with detailed information.
 *
 * @param[in] function  	- the function where the error occurred.
 * @param[in] file      	- the source file where the error occurred.
 * @param[in] line      	- the line in the file where the error occurred.
 * @param[in] error	    	- the error code.
 */
void err_full_msg(const char *function, const char *file,
		int line, int error);

/**
 * Prints the error message respective to an error code.
 *
 * @param[out] e			- the error occurred.
 * @param[out] msg			- the error message.
 */
void err_get_msg(err_t *e, char **msg);

#endif

/**
 * Returns the code returned by the last function call and resets the current
 * code.
 *
 * @returns ERR_OK if no errors occurred in the function, ERR_ERR otherwise.
 */
int err_get_code(void);

#endif /* !RELIC_ERROR_H */
