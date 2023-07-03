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

#ifndef AT_MB_IIE_GRIDVALS_BASIC__SRC_H__
#define AT_MB_IIE_GRIDVALS_BASIC__SRC_H__



/* integral-identity estimators for grid point values */



#include "at_mb_iie_gridvals_basic.h"


static void at_mb_iie_gridvals_item__clear(at_mb_iie_gridvals_item_t *item)
{
  item->lnz = 0.0;
  item->e = 0.0;
  item->cv = 0.0;
  item->quality_bits = 0;
}



void at_mb_iie_gridvals__conf_init(at_mb_iie_gridvals_t *gridvals, at_mb_t *mb,
    at_utils_conf_t *conf)
{
  int i, n = mb->distr->domain->n;

  gridvals->n = n;

  at_utils__new_arr(gridvals->items, n+1, at_mb_iie_gridvals_item_t);
  for (i = 0; i <= n; i++) {
    at_mb_iie_gridvals_item__clear(gridvals->items + i);
  }
}


void at_mb_iie_gridvals__clear(at_mb_iie_gridvals_t *gridvals)
{
  int i, n = gridvals->n;

  for (i = 0; i <= n; i++) {
    at_mb_iie_gridvals_item__clear(gridvals->items + i);
  }

}



void at_mb_iie_gridvals__finish(at_mb_iie_gridvals_t *gridvals)
{
  free(gridvals->items);
}


#endif
