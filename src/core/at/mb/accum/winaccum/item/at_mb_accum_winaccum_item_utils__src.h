/* 
 * Copyright (C) 2010-2023  AT-Engine Developers
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

#ifndef AT_MB_ACCUM_WINACCUM_ITEM_UTILS__SRC_H__
#define AT_MB_ACCUM_WINACCUM_ITEM_UTILS__SRC_H__


#include "at_mb_accum_winaccum_item.h"

#include "../../../sm/at_mb_sm.h"



at_mb_sm_t *at_mb_accum_winaccum_item__get_sums(
    at_mb_accum_winaccum_item_t *item,
    int j)
{
  return item->sums + j - item->js;
}



void at_mb_accum_winaccum_item__normalize(
    at_mb_accum_winaccum_item_t *item)
{
  double fac;
  int j, m = item->jt - item->js;

  if (fabs(item->amplifier - 1.0) < 1e-6) {
    return;
  }

  fac = 1.0 / item->amplifier;

  for (j = 0; j < m; j++) {
    at_mb_sm__scale(item->sums + j, fac);
  }

  item->amplifier = 1.0;
}



double at_mb_accum_winaccum_item__calc_total(
    at_mb_accum_winaccum_item_t *item)
{
  double total = 0.0;
  int j, m = item->jt - item->js;

  for (j = 0; j < m; j++) {
    total += item->sums[j].s;
  }

  return total / item->amplifier;
}



#endif
