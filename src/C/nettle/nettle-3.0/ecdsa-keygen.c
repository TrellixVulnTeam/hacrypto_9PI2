/* ecdsa-keygen.c

   Copyright (C) 2013 Niels Möller

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/

/* Development of Nettle's ECC support was funded by the .SE Internet Fund. */

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <stdlib.h>

#include "ecdsa.h"
#include "ecc-internal.h"
#include "nettle-internal.h"

void
ecdsa_generate_keypair (struct ecc_point *pub,
			struct ecc_scalar *key,
			void *random_ctx, nettle_random_func *random)
{
  TMP_DECL(p, mp_limb_t, 3*ECC_MAX_SIZE + ECC_MUL_G_ITCH (ECC_MAX_SIZE));
  mp_size_t itch = 3*pub->ecc->size + ECC_MUL_G_ITCH (pub->ecc->size);

  assert (key->ecc == pub->ecc);

  TMP_ALLOC (p, itch);

  ecc_modq_random (key->ecc, key->p, random_ctx, random, p);
  ecc_mul_g (pub->ecc, p, key->p, p + 3*pub->ecc->size);
  ecc_j_to_a (pub->ecc, 1, pub->p, p, p + 3*pub->ecc->size);
}
