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

#ifndef AT_MB_ACCUM_WINACCUM_BASIC__SRC_H__
#define AT_MB_ACCUM_WINACCUM_BASIC__SRC_H__


#include "at_mb_accum_winaccum_basic.h"
#include "at_mb_accum_winaccum_item.h"

#include "../../sm/at_mb_sm.h"

#include "../../../../zcom/zcom.h"




/* normalize damping weight back to 1.0, averages are not affected
 * if i < 0, do for all estimators */
void at_mb_accum_winaccum__normalize(at_mb_accum_winaccum_t *winaccum, int i)
{
  int i0, i1;

  if (i >= 0) {
    i0 = i1 = i;
  } else { /* normalize all estimators, if i < 0 */
    i0 = 0;
    i1 = winaccum->n;
  }

  for (i = i0; i <= i1; i++) { /* loop over estimators */
    at_mb_accum_winaccum_item__normalize(winaccum->items+i);
  }

}



void at_mb_accum_winaccum__add(
    at_mb_accum_winaccum_t *winaccum,
    int j,
    double invwf, double e, int do_2nd,
    at_mb_shk_t *shk, double total_visits)
{
  int i, l;
  at_mb_win_t *win = winaccum->win;
  at_mb_win_ids_t *wj = win->bin2wins + j;

  for (l = 0; l < wj->count; l++) { /* loop over affected estimators */

    // update the ith window accumulator
    i = wj->ids[l];

    at_mb_accum_winaccum_item__add(
        winaccum->items + i, j,
        invwf, e, do_2nd, shk, total_visits);
  }

}



/* compute total weighted number of visits to T. window of bin i */
void at_mb_accum_winaccum__calc_win_total(at_mb_accum_winaccum_t *winaccum, double *win_total)
{
  int i;

  for (i = 0; i < winaccum->n; i++) {
    win_total[i] = at_mb_accum_winaccum_item__calc_total(winaccum->items + i);
  }

}



void at_mb_accum_winaccum__init(at_mb_accum_winaccum_t *winaccum, int n, at_mb_win_t *win, unsigned flags)
{
  int i;

  winaccum->n = n;

  winaccum->win = win;

  winaccum->flags = flags;

  zcom_util__exit_if ((winaccum->items = (at_mb_accum_winaccum_item_t *) calloc(n, sizeof(at_mb_accum_winaccum_item_t))) == NULL,
      "no memory! var: winaccum->items, type: at_mb_sm_t\n");

  for (i = 0; i < n; i++) {
    at_mb_accum_winaccum_item__init(winaccum->items + i, i, winaccum->win);
  }

}


void at_mb_accum_winaccum__clear(at_mb_accum_winaccum_t *winaccum)
{
  int i;

  for (i = 0; i < winaccum->n; i++) {
    at_mb_accum_winaccum_item__clear(winaccum->items + i);
  }

}



void at_mb_accum_winaccum__finish(at_mb_accum_winaccum_t *winaccum)
{
  int i;

  for (i = 0; i < winaccum->n; i++) {
    at_mb_accum_winaccum_item__finish(winaccum->items+i);
  }

  free(winaccum->items);
}



#endif