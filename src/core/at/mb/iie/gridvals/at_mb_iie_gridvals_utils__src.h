/* 
 * Copyright (C) 2010-2023  At-engine Developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef AT_MB_IIE_GRIDVALS_UTILS__SRC_H__
#define AT_MB_IIE_GRIDVALS_UTILS__SRC_H__



/* integral-identity estimators for grid point values */



#include "at_mb_iie_gridvals_utils.h"




/* set quality bit */
void at_mb_iie_gridvals_item__set_quality_bit(
    at_mb_iie_gridvals_item_t *item,
    unsigned mask, int on)
{
  if (mask) {
    if (on) {
      item->quality_bits |= mask;
    } else {
      item->quality_bits &= ~mask;
    }
  }
}


/* translate the quality bits into an array of 0-1 characters */
char *at_mb_iie_gridvals_item__quality_bits_to_string(const at_mb_iie_gridvals_item_t *item)
{
  static char buf[64]; /* has to be static to be the return value */
  int  cnt = 0;

  buf[cnt++] = '1';
  buf[cnt++] = (char)((item->quality_bits & AT_MB_IIE_GRIDVALS__QUALITY_BIT_EHAT) ? '1' : '0');
  buf[cnt++] = (char)((item->quality_bits & AT_MB_IIE_GRIDVALS__QUALITY_BIT_CV  ) ? '1' : '0');
  buf[cnt++] = (char)((item->quality_bits & AT_MB_IIE_GRIDVALS__QUALITY_BIT_LNZ ) ? '1' : '0');
  buf[cnt] = '\0';

  return buf;
}




#endif
