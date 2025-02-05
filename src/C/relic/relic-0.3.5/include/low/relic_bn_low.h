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
 * Interface of the low-level multiple precision integer arithmetic module.
 *
 * All functions assume that the destination has enough capacity to store
 * the result of the computation.
 *
 * @version $Id: relic_bn_low.h 1534 2013-08-27 22:33:52Z dfaranha $
 * @ingroup bn
 */

#ifndef RELIC_BN_LOW_H
#define RELIC_BN_LOW_H

/*============================================================================*/
/* Constant definitions                                                       */
/*============================================================================*/

#ifdef ASM

#include "relic_conf.h"

#if (BN_PRECI % WORD) > 0
#define BN_DIGS	(BN_PRECI/WORD + 1)
#else
#define BN_DIGS	(BN_PRECI/WORD)
#endif

#if BN_MAGNI == DOUBLE
#define BN_SIZE	(2 * BN_DIGS + 2)
#elif BN_MAGNI == CARRY
#define BN_SIZE	((BN_DIGS + 1)
#elif BN_MAGNI == SINGLE
#define BN_SIZE	(BN_DIGS)
#endif

#else

#include "relic_types.h"

/*============================================================================*/
/* Function prototypes                                                        */
/*============================================================================*/

/**
 * Adds a digit to a digit vector. Computes c = a + digit.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the first digit vector to add.
 * @param[in] digit			- the digit to add.
 * @param[in] size			- the number of digits in the first operand.
 * @return the carry of the last digit addition.
 */
dig_t bn_add1_low(dig_t *c, const dig_t *a, const dig_t digit, const int size);

/**
 * Adds two digit vectors of the same size. Computes c = a + b.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the first digit vector to add.
 * @param[in] b				- the second digit vector to add.
 * @param[in] size			- the number of digits to add.
 * @return the carry of the last digit addition.
 */
dig_t bn_addn_low(dig_t *c, const dig_t *a, const dig_t *b, int size);

/**
 * Subtracts a digit from a digit vector. Computes c = a - digit.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the digit vector.
 * @param[in] digit			- the digit to subtract.
 * @param[in] size			- the number of digits in a.
 * @return the carry of the last digit subtraction.
 */
dig_t bn_sub1_low(dig_t *c, const dig_t *a, dig_t digit, int size);

/**
 * Subtracts a digit vector from another digit vector. Computes c = a - b.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the digit vector.
 * @param[in] b				- the digit vector to subtract.
 * @param[in] size			- the number of digits to subtract.
 * @return the carry of the last digit subtraction.
 */
dig_t bn_subn_low(dig_t *c, const dig_t *a, const dig_t *b, int size);

/**
 * Compares two digits.
 *
 * @param[in] a				- the first digit to compare.
 * @param[in] b				- the second digit to compare.
 * @return BN_LT if a < b, BN_EQ if a == b and BN_GT if a > b.
 */
int bn_cmp1_low(dig_t a, dig_t b);

/**
 * Compares two digit vectors of the same size.
 *
 * @param[in] a				- the first digit vector to compare.
 * @param[in] b				- the second digit vector to compare.
 * @param[in] size			- the number of digits to compare.
 * @return BN_LT if a < b, BN_EQ if a == b and BN_GT if a > b.
 */
int bn_cmpn_low(const dig_t *a, const dig_t *b, int size);

/**
 * Shifts a digit vector to the left by 1 bit. Computes c = a << 1.
 *
 * @param[out] c			- the result
 * @param[in] a				- the digit vector to shift.
 * @param[in] size			- the number of digits to shift.
 * @return the carry of the last digit shift.
 */
dig_t bn_lsh1_low(dig_t *c, const dig_t *a, int size);

/**
 * Shifts a digit vector to the left by an amount smaller than a digit. Computes
 * c = a << bits.
 *
 * @param[out] c			- the result
 * @param[in] a				- the digit vector to shift.
 * @param[in] size			- the number of digits to shift.
 * @param[in] bits			- the shift amount.
 * @return the carry of the last digit shift.
 */
dig_t bn_lshb_low(dig_t *c, const dig_t *a, int size, int bits);

/**
 * Shifts a digit vector to the left by some digits.
 * Computes c = a << (digits * DIGIT).
 *
 * @param[out] c			- the result.
 * @param[in] a				- the multiple precision integer to shift.
 * @param[in] size			- the number of digits to shift.
 * @param[in] digits		- the shift amount.
 */
void bn_lshd_low(dig_t *c, const dig_t *a, int size, int digits);

/**
 * Shifts a digit vector to the right by 1 bit. Computes c = a >> 1.
 *
 * @param[out] c			- the result
 * @param[in] a				- the digit vector to shift.
 * @param[in] size			- the number of digits to shift.
 * @return the carry of the last digit shift.
 */
dig_t bn_rsh1_low(dig_t *c, const dig_t *a, int size);

/**
 * Shifts a digit vector to the right by an amount smaller than a digit.
 * Computes c = a >> bits.
 *
 * @param[out] c			- the result
 * @param[in] a				- the digit vector to shift.
 * @param[in] size			- the number of digits to shift.
 * @param[in] bits			- the shift amount.
 * @return the carry of the last digit shift.
 */
dig_t bn_rshb_low(dig_t *c, const dig_t *a, int size, int bits);

/**
 * Shifts a digit vector to the right by some digits.
 * Computes c = a >> (digits * DIGIT).
 *
 * @param[out] c			- the result.
 * @param[in] a				- the multiple precision integer to shift.
 * @param[in] size			- the number of digits to shift.
 * @param[in] digits		- the shift amount.
 */
void bn_rshd_low(dig_t *c, const dig_t *a, int size, int digits);

/**
 * Multiplies a digit vector by a digit and adds this result to another digit
 * vector. Computes c = c + a * digit.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the digit vector to multiply.
 * @param[in] digit			- the digit to multiply.
 * @param[in] size			- the number of digits to multiply.
 * @return the carry of the addition.
 */
dig_t bn_mula_low(dig_t *c, const dig_t *a, dig_t digit, int size);

/**
 * Multiplies a digit vector by a digit and stores this result in another digit
 * vector. Computes c = a * digit.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the first digit vector to multiply.
 * @param[in] digit			- the digit to multiply.
 * @param[in] size			- the number of digits to multiply.
 * @return the most significant digit.
 */
dig_t bn_mul1_low(dig_t *c, const dig_t *a, dig_t digit, int size);

/**
 * Multiplies two digit vectors of the same size. Computes c = a * b.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the first digit vector to multiply.
 * @param[in] b				- the second digit vector to multiply.
 * @param[in] size			- the number of digits to multiply.
 */
void bn_muln_low(dig_t *c, const dig_t *a, const dig_t *b, int size);

/**
 * Multiplies two digit vectors of different sizes, with sizea > sizeb. Computes
 * c = a * b. This function outputs as result only the digits between low and
 * high, inclusive, with high > sizea and low < sizeb.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the first digit vector to multiply.
 * @param[in] b				- the second digit vector to multiply.
 * @param[in] sizea			- the number of digits in the first operand.
 * @param[in] sizeb			- the number of digits in the second operand.
 * @param[in] low			- the first digit to compute.
 * @param[in] high			- the last digit to compute.
 */
void bn_muld_low(dig_t *c, const dig_t *a, int sizea, const dig_t *b, int sizeb,
		int low, int high);

/**
 * Squares a digit vector and adds this result to another digit vector.
 * Computes c = c + a * a.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the digit vector to square.
 * @param[in] size			- the number of digitss to square.
 */
void bn_sqra_low(dig_t *c, const dig_t *a, int size);

/**
 * Squares a digit vector. Computes c = a * a.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the digit vector to square.
 * @param[in] size			- the number of digits to square.
 */
void bn_sqrn_low(dig_t *c, const dig_t *a, int size);

/**
 * Divides a digit vector by another digit vector. Computes c = floor(a / b) and
 * d = a mod b. The dividend and the divisor are destroyed inside the function.
 *
 * @param[out] c			- the quotient.
 * @param[out] d			- the remainder.
 * @param[in,out] a			- the dividend.
 * @param[in] sizea			- the size of the dividend.
 * @param[in,out] b			- the divisor.
 * @param[in] sizeb			- the size of the divisor.
 */
void bn_divn_low(dig_t *c, dig_t *d, dig_t *a, int sizea, dig_t *b,
		int sizeb);

/**
 * Divides a digit vector by a digit. Computes c = floor(a / digit) and
 * d = a mod digit.
 *
 * @param[out] c			- the quotient.
 * @param[out] d			- the remainder.
 * @param[in] a				- the dividend.
 * @param[in] size			- the size of the dividend.
 * @param[in] digit				- the divisor.
 */
void bn_div1_low(dig_t *c, dig_t *d, const dig_t *a, int size, dig_t digit);

/**
 * Reduces a digit vector modulo m by Montgomery's algorithm.
 *
 * @param[out] c			- the result.
 * @param[in] a				- the digit vector to reduce.
 * @param[in] sizea			- the number of digits to reduce
 * @param[in] m				- the modulus.
 * @param[in] sizem			- the size of the modulus.
 * @param[in] u				- the reciprocal of the modulus.
 */
void bn_modn_low(dig_t *c, const dig_t *a, int sizea, const dig_t *m, int sizem,
		dig_t u);

#endif /* !ASM */

#endif /* !RELIC_BN_LOW_H */
