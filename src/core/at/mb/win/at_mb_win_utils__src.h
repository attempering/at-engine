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

#ifndef AT_MB_WIN_UTILS__SRC_H__
#define AT_MB_WIN_UTILS__SRC_H__


#include "at_mb_win_utils.h"

#include "../../../zcom/zcom.h"



void at_mb_win__make_unres_windows_for_grid_estimators(
    int n, const double *barr, double beta_del,
    int bwmod, double bwdel,
    int *js_grid_unres, int *jt_grid_unres)
{
  int i, js, jt, idel;
  double bet, dbeta = 0.0;

  zcom_utils__exit_if (js_grid_unres == NULL || jt_grid_unres == NULL,
      "null pointer, js: %p, jt: %p",
      js_grid_unres, jt_grid_unres);

  for (i = 0; i <= n; i++) {
    bet = barr[i];
    switch (bwmod) {
      case 0: dbeta = bwdel; break;
      case 1: dbeta = bwdel * bet; break;
      case 2: dbeta = bwdel * (bet * bet); break;
      default:
        zcom_utils__exit_if(1, "bad bwmod=%d\n", bwmod);
        break;
    }

    idel = (int)(dbeta/beta_del + 0.5);

    /* although idel = 0 should be fine */
    if (idel < 1) {
      idel = 1;
    }

    if ((js = i - idel) < 0) {
      js = 0;
    }

    if ((jt = i + idel) > n) {
      jt = n;
    }

    zcom_utils__exit_if (i < js || i > jt,
      "bad window %d (%d,%d)\n", i, js, jt);

    js_grid_unres[i] = js;
    jt_grid_unres[i] = jt;
  }
}


void at_mb_win__make_windows_for_bin_estimators(int n, int symmetric,
    const int *js_grid_unres, const int *jt_grid_unres,
    int *js_bin, int *jt_bin)
{
  int i;

  for (i = 0; i < n; i++) {

    if (symmetric) {

      int di1 = i - js_grid_unres[i];
      int di2 = jt_grid_unres[i+1] - (i + 1);

      /* choose the smaller from di1 and di2 */
      int di = (di1 > di2) ? di2 : di1;

      js_bin[i] = i - di;
      jt_bin[i] = i + 1 + di;

    } else {

      js_bin[i] = js_grid_unres[i];
      jt_bin[i] = jt_grid_unres[i + 1];

    }

#ifdef MB_DBG__
    fprintf(stderr, "bin %d, [%d, %d)\n", i, js_bin[i], jt_bin[i]);
#endif

  }
}



/* make a restricted window for grid point i */
static void at_mb_win__make_res_window_for_grid_estimators(int n, int i,
    int *js_bin, int *jt_bin, int *js, int *jt)
{
  int win_id = (i < n) ? i : (i - 1);

  *js = js_bin[win_id];
  *jt = jt_bin[win_id];

  // for the first grid point (i == 0, win_id == 0)
  // the bin-window contains a single bin and cannot be made symmetric
  // we will leave the window as is.
  //
  // for the last two grid points (i == n-1 or n, win_id == n - 1)
  // the bin-window (win_id == n-1) also contains only a single bin
  // we will also leave it as is
  //
  // for the rest of grid points in the middle
  // we can tweak them a bit to make them as symmetric possible.
  //
  if (win_id > 0 && win_id < n-1) {

    // Example: n = 6
    //
    // bins:    | 0 | 1 | 2 | 3 | 4 | 5 |
    //          ^   ^   ^   ^   ^   ^   ^
    // grids:   0   1   2   3   4   5   6
    //
    // for grid point i == 2, the corresponding bin-window of bin 2 is probably [0, 5)
    // containing bins 0, 1, 2, 3, 4
    // the grid-window should be [0, 4) containing bins 0, 1 & 2, 3
    //
    // for grid point i == 3, the corresponding bin-window of bin 2 is
    // probably [1, 6) containing bins 1, 2, 3, 4, 5
    // the grid-window should be [1, 5) containing bins 1, 2 & 3, 4
    //
    // The moral of this example is that we should shrink the right boundary
    // if i < n/2, or shrink the left boundary otherwise
    //
    if (*js < *jt - 1) {

      if (i < n/2) {
        (*jt)--;
      } else {
        (*js)++;
      }

    }

  }

  zcom_utils__exit_if (*jt - *js <= 0, "empty window (%d,%d) for %d\n", *js, *jt, i);
}


void at_mb_win__make_res_windows_for_grid_estimators(int n,
    int *js_bin, int *jt_bin,
    int *js_grid_res, int *jt_grid_res)
{
  int i;

  for (i = 0; i <= n; i++) {
    at_mb_win__make_res_window_for_grid_estimators(
        n, i,
        js_bin, jt_bin,
        &js_grid_res[i], &jt_grid_res[i]);
  }
}



static void at_mb_win__calc_window_counts(at_mb_win_t *win)
{
  int i, j;
  int n = win->n;

  for (j = 0; j < n; j++) {
    win->bin2wins[j].count = 0;
  }

  for (i = 0; i < n; i++) {
    int js = win->js_bin[i];
    int jt = win->jt_bin[i];

    for (j = js; j < jt; j++) {
      win->bin2wins[j].count++;
    }
  }
}



int at_mb_win__init_bin2wins(at_mb_win_t *win)
{
  int n = win->n;
  int i, j;
  at_mb_win_ids_t *wj;

  zcom_utils__exit_if ((win->bin2wins = (at_mb_win_ids_t *) calloc(n, sizeof(*win->bin2wins))) == NULL,
      "no memory for win->bin2wins of size %d", n);

  at_mb_win__calc_window_counts(win);

  for (j = 0; j < n; j++) {
    wj = win->bin2wins + j;
    
    wj->curr_id_ = 0;

    zcom_utils__exit_if ((wj->ids = (int *) calloc(wj->count, sizeof(int))) == NULL,
        "no memory for win->bin2wins[%d].ids count %d\n", j, wj->count);
  }

  for (i = 0; i < n; i++) {
    int js = win->js_bin[i];
    int jt = win->jt_bin[i];

    for (j = js; j < jt; j++) {
      wj = win->bin2wins + j;

      zcom_utils__exit_if (wj->curr_id_ >= wj->count,
          "win->bin2wins[%d].curr_id: %d >= %d, m: %d, win: %d, bin: %d, (js, jt) = [%d, %d).\n",
          j, wj->curr_id_, wj->count, i, j, js, jt);

      wj->ids[wj->curr_id_++] = i;
    }

  }

  return 0;
}


#endif
