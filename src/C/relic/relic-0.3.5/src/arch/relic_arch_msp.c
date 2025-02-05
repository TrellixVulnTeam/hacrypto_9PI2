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
 * Implementation of MSP-dependent routines.
 *
 * @version $Id: relic_arch_msp.c 1519 2013-08-27 15:51:45Z dfaranha $
 * @ingroup arch
 */

#include "relic_core.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void arch_init(void) {
}

void arch_clean(void) {
}

#ifdef __MSP430__
/* Support for MSPGCC with custom MSPsim simulator. */

static volatile unsigned char TEST_TEXTOUT __asm__("0x01b0");
static volatile unsigned int BENCH_CYCLES_0 __asm__("0x01b2");
static volatile unsigned int BENCH_CYCLES_1 __asm__("0x01b4");
static volatile unsigned int BENCH_CYCLES_2 __asm__("0x01b6");
static volatile unsigned int BENCH_CYCLES_3 __asm__("0x01b8");

union cycles_t {
    unsigned long long cycles;
    struct {
        unsigned int e0;
        unsigned int e1;
        unsigned int e2;
        unsigned int e3;
    } e;
};

int putchar(int c) {
    TEST_TEXTOUT = (char) c;
    return c;
}

unsigned long long arch_cycles() {
    union cycles_t cycles;
    cycles.e.e0 = BENCH_CYCLES_0;
    cycles.e.e1 = BENCH_CYCLES_1;
    cycles.e.e2 = BENCH_CYCLES_2;
    cycles.e.e3 = BENCH_CYCLES_3;
    return cycles.cycles;
}

#endif

#if __ICC430__
/* Support for IAR using simulator with custom macro. */

volatile ull_t  __cycles = 0;

ull_t arch_cycles() {
	return __cycles;
}

#endif
