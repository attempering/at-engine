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

  zcom_util__exit_if(!winaccum->enabled, "winaccum is disabled\n");

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
  //fprintf(stderr, "at_mb_accum_winaccum__add() winaccum %p\n", winaccum);
  //fprintf(stderr, "at_mb_accum_winaccum__add() win %p\n", winaccum->win);
  //getchar();
  zcom_util__exit_if(!winaccum->enabled, "winaccum is disabled\n");

  int i, l;
  at_mb_win_t *win = winaccum->win;
  at_mb_win_ids_t *wj = win->bin2wins + j;

  //fprintf(stderr, "at_mb_accum_winaccum__add() win %p, bin2wins %p\n", win, win->bin2wins); getchar();

  for (l = 0; l < wj->count; l++) { /* loop over affected estimators */

    // update the ith window accumulator
    i = wj->ids[l];

    //fprintf(stderr, "at_mb_accum_winaccum__add() j %d/%d, l %d, i %d, %p, %s:%d\n", j, winaccum->n, l, i, winaccum->items, __FILE__, __LINE__);

    at_mb_accum_winaccum_item__add(
        winaccum->items + i, j,
        invwf, e, do_2nd, shk, total_visits);
  }
  //getchar();

}



/* compute total weighted number of visits to T. window of bin i */
void at_mb_accum_winaccum__calc_win_total(at_mb_accum_winaccum_t *winaccum, double *win_total)
{
  int i;

  zcom_util__exit_if(!winaccum->enabled, "winaccum is disabled\n");

  for (i = 0; i < winaccum->n; i++) {
    win_total[i] = at_mb_accum_winaccum_item__calc_total(winaccum->items + i);
  }

}



void at_mb_accum_winaccum__cfg_init(at_mb_accum_winaccum_t *winaccum,
    int n, at_mb_win_t *win,
    zcom_cfg_t *cfg, at_bool_t verbose)
{
  int i;

  //fprintf(stderr, "n %d, %s:%d\n", n, __FILE__, __LINE__);

  winaccum->n = n;

  winaccum->win = win;
  zcom_util__exit_if (win == NULL,
      "at_mb_win_t *win is NULL\n");

  winaccum->enabled = AT__TRUE;
  if (0 != zcom_cfg__get(cfg, &winaccum->enabled, "mbest-damp,mb-use-win-accum", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.accum.winaccum: assuming default mb->accum->winaccum->enabled = 1, key: mb-use-win-accum\n");
  }

  if (winaccum->enabled) {

    zcom_util__exit_if ((winaccum->items = (at_mb_accum_winaccum_item_t *) calloc(n, sizeof(at_mb_accum_winaccum_item_t))) == NULL,
        "Error@at.mb.accum.winaccum: no memory! var: winaccum->items, type: at_mb_sm_t\n");

    for (i = 0; i < n; i++) {
      at_mb_accum_winaccum_item__init(winaccum->items + i, i, winaccum->win);
    }

  } else {

    winaccum->items = NULL;

  }

}


void at_mb_accum_winaccum__clear(at_mb_accum_winaccum_t *winaccum)
{
  int i;

  if (!winaccum->enabled) return;

  for (i = 0; i < winaccum->n; i++) {
    at_mb_accum_winaccum_item__clear(winaccum->items + i);
  }

}



void at_mb_accum_winaccum__finish(at_mb_accum_winaccum_t *winaccum)
{
  int i;

  if (!winaccum->enabled) return;

  for (i = 0; i < winaccum->n; i++) {
    at_mb_accum_winaccum_item__finish(winaccum->items+i);
  }

  free(winaccum->items);
}



#endif