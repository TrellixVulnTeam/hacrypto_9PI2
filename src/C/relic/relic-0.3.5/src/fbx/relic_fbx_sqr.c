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
 * Implementation of squaring in binary fields extensions.
 *
 * @version $Id: relic_fbx_sqr.c 1489 2013-06-02 18:33:31Z dfaranha $
 * @ingroup fbx
 */

#include "relic_core.h"
#include "relic_fbx.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void fb2_sqr(fb2_t c, fb2_t a) {
	fb_sqr(c[1], a[1]);
	fb_sqr(c[0], a[0]);
	fb_add(c[0], c[0], c[1]);
}

void fb4_sqr(fb4_t c, fb4_t a) {
	fb_sqr(c[3], a[3]);
	fb_sqr(c[2], a[2]);
	fb_sqr(c[1], a[1]);
	fb_sqr(c[0], a[0]);
	fb_add(c[0], c[0], c[1]);
	fb_add(c[0], c[0], c[3]);
	fb_add(c[1], c[1], c[2]);
	fb_add(c[2], c[2], c[3]);
}
