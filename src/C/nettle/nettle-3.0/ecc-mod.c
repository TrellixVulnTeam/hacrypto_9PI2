/* ecc-mod.c

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

#include "ecc-internal.h"

/* Computes r mod m, where m is of size mn. bp holds B^mn mod m, as mn
   limbs, but the upper mn - bn libms are zero. */
void
ecc_mod (mp_limb_t *rp, mp_size_t rn, mp_size_t mn,
	 const mp_limb_t *bp, mp_size_t bn,
	 const mp_limb_t *b_shifted, unsigned shift)
{
  mp_limb_t hi;
  mp_size_t sn = mn - bn;
  mp_size_t i;

  assert (sn > 0);

  /* FIXME: Could use mpn_addmul_2. */
  /* Eliminate sn = mn - bn limbs at a time */
  if (bp[bn-1] < ((mp_limb_t) 1 << (GMP_NUMB_BITS - 1)))
    {
      /* Multiply sn + 1 limbs at a time, so we get a mn+1 limb
	 product. Then we can absorb the carry in the high limb */
      while (rn > 2 * mn - bn)
	{
	  rn -= sn;

	  for (i = 0; i <= sn; i++)
	    rp[rn+i-1] = mpn_addmul_1 (rp + rn - mn - 1 + i, bp, bn, rp[rn+i-1]);
	  rp[rn-1] = rp[rn+sn-1]
	    + mpn_add_n (rp + rn - sn - 1, rp + rn - sn - 1, rp + rn - 1, sn);
	}
      goto final_limbs;
    }
  else
    {
      while (rn >= 2 * mn - bn)
	{
	  rn -= sn;

	  for (i = 0; i < sn; i++)
	    rp[rn+i] = mpn_addmul_1 (rp + rn - mn + i, bp, bn, rp[rn+i]);
				     
	  hi = mpn_add_n (rp + rn - sn, rp + rn - sn, rp + rn, sn);
	  hi = cnd_add_n (hi, rp + rn - mn, bp, mn);
	  assert (hi == 0);
	}
    }

  if (rn > mn)
    {
    final_limbs:
      sn = rn - mn;
      
      for (i = 0; i < sn; i++)
	rp[mn+i] = mpn_addmul_1 (rp + i, bp, bn, rp[mn+i]);

      hi = mpn_add_n (rp + bn, rp + bn, rp + mn, sn);
      hi = sec_add_1 (rp + bn + sn, rp + bn + sn, mn - bn - sn, hi);
    }

  if (shift > 0)
    {
      /* Combine hi with top bits, add in */
      hi = (hi << shift) | (rp[mn-1] >> (GMP_NUMB_BITS - shift));
      rp[mn-1] = (rp[mn-1] & (((mp_limb_t) 1 << (GMP_NUMB_BITS - shift)) - 1))
	+ mpn_addmul_1 (rp, b_shifted, mn-1, hi);
    }
  else
    {
      hi = cnd_add_n (hi, rp, bp, mn);
      assert (hi == 0);
    }
}
