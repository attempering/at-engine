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

#ifndef AT_MB_ACCUM_BASIC__SRC_H__
#define AT_MB_ACCUM_BASIC__SRC_H__


#include "at_mb_accum.h"

#include "winaccum/at_mb_accum_winaccum.h"

#include "../sm/at_mb_sm.h"

#include "../../../zcom/zcom.h"



/* return the origin of sums for the window estimator at win_id */
static at_mb_sm_t *at_mb_accum__get_sums0(at_mb_accum_t *accum)
{
  return accum->sums;
}


/* return the address of sums for the window estimator at win_id */
at_mb_sm_t *at_mb_accum__get_sums(at_mb_accum_t *accum, int j)
{
  return accum->sums + j;
}



at_mb_sm_t *at_mb_accum__get_proper_sums0_and_winaccum_item(
    at_mb_accum_t *accum, int win_id,
    at_mb_accum_winaccum_item_t **winaccum_item)
{

  if (accum->winaccum->enabled) {

    if (win_id == accum->n) {
      win_id = accum->n - 1;
    }

    *winaccum_item = accum->winaccum->items + win_id;

    return (*winaccum_item)->sums - (*winaccum_item)->js;

  } else {

    *winaccum_item = NULL;

    return at_mb_accum__get_sums0(accum);

  }

}



at_mb_sm_t *at_mb_accum__get_proper_sums(at_mb_accum_t *accum, int win_id, int ib)
{
  at_mb_accum_winaccum_item_t *winaccum_item;

  at_mb_sm_t *sm0 = at_mb_accum__get_proper_sums0_and_winaccum_item(
      accum, win_id, &winaccum_item);

  return sm0 + ib;
}



double at_mb_accum__get_window_visits(at_mb_accum_t *accum, int win_id)
{
  if (accum->winaccum->enabled) {
    return accum->winaccum->items[win_id].visits;
  } else {

    double visits = 0.0;
    int ib;
    
    for (ib = accum->win->js_bin[win_id]; ib < accum->win->jt_bin[win_id]; ib++) {
      visits += accum->sums[ib].s;
    }

    return visits;

  }

}



void at_mb_accum__add(at_mb_accum_t *accum, int ib, double invwf, double e, int do_2nd,
    at_mb_shk_t *shk, double total_visits)
{
  // for the plain accumulators
  at_mb_sm_t *sm = at_mb_accum__get_sums(accum, ib);
  //printf("%g %g %g %g\n", sm->s, sm->se, sm->se2, sm->se3);
  at_mb_sm__add(sm, invwf, e, do_2nd);

  // for the window accumulators
  if (accum->winaccum->enabled) { /* add to damping data */
    at_mb_accum_winaccum__add(
        accum->winaccum, ib,
        invwf, e, do_2nd, shk, total_visits);
  }

}


void at_mb_accum__calc_win_total(at_mb_accum_t *accum)
{
  if (accum->winaccum->enabled) {
    at_mb_accum_winaccum__calc_win_total(accum->winaccum, accum->win_total);
  } else {

    int i;

    for (i = 0; i < accum->n; i++) {
      int j;
      int js_bin = accum->win->js_bin[i];
      int jt_bin = accum->win->jt_bin[i];
      double total = 0.0;

      for (j = js_bin; j < jt_bin; j++) {
        total += accum->sums[j].s;
      }
      
      accum->win_total[i] = total;
    }

  }
}



void at_mb_accum__cfg_init(at_mb_accum_t *accum, int n, at_mb_win_t *win,
    zcom_cfg_t *cfg, at_bool_t verbose)
{
  int i;

  accum->n = n;

  accum->win = win;

  /* sums: normal data */
  zcom_util__exit_if ((accum->sums = (at_mb_sm_t *) calloc((accum->n + 1), sizeof(at_mb_sm_t))) == NULL,
      "no memory! var: accum->sums, type: at_mb_sm_t\n");

  for (i = 0; i < accum->n; i++) {
    at_mb_sm__init(accum->sums+i);
  }

  at_mb_accum_winaccum__cfg_init(accum->winaccum, n, win, cfg, verbose);

  /* win_total: total of sum.s over a multiple-bin temperature window */
  zcom_util__exit_if ((accum->win_total = (double *) calloc(n, sizeof(double))) == NULL,
    "no memory! var: accum->win_total, type: double\n");

  for (i = 0; i < n; i++) {
    accum->win_total[i] = 0.0;
  }

}


void at_mb_accum__clear(at_mb_accum_t *accum)
{
  int i;

  if (accum->sums != NULL) {
    for (i = 0; i < accum->n; i++) {
      at_mb_sm__clear(accum->sums + i);
    }
  }

  for (i = 0; i < accum->n; i++) {
    accum->win_total[i] = 0.0;
  }

  if (accum->winaccum->enabled) {
    at_mb_accum_winaccum__clear(accum->winaccum);
  }

}



void at_mb_accum__finish(at_mb_accum_t *accum)
{
  free(accum->sums);

  free(accum->win_total);

  if (accum->winaccum->enabled) {
    //fprintf(stderr, "at_mb_accum__finish() %p %p\n", accum, accum->winaccum); getchar();
    at_mb_accum_winaccum__finish(accum->winaccum);
  }
}



#endif