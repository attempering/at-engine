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

#ifndef AT_MB_ACCUM_WINACCUM_ITEM__SRC_H__
#define AT_MB_ACCUM_WINACCUM_ITEM__SRC_H__


#include "at_mb_accum_winaccum_item.h"

#include "at_mb_accum_winaccum_item_utils__src.h"
#include "io/at_mb_accum_winaccum_item_io__src.h"

#include "../../../sm/at_mb_sm.h"
#include "../../../../../zcom/zcom.h"



void at_mb_accum_winaccum_item__add(
    at_mb_accum_winaccum_item_t *item,
    int ib,
    double invwf, double e, int do_2nd,
    at_mb_shk_t *shk,
    double total_visits)
{
  const double amplifier_threshold = 2.0;
  double ginvwf;
  at_mb_sm_t *sm;
  double inv_gamma;

  inv_gamma = at_mb_shk__calc_inv_gamma(shk, total_visits, item->i);

  //fprintf(stderr, "inv_gamma %g, shk %p\n", inv_gamma, shk); getchar();

  // apply adaptive averaging
  item->amplifier *= inv_gamma;

  ginvwf = item->amplifier * invwf; /* multiplied by the accumulated 1/gamma */

  sm = at_mb_accum_winaccum_item__get_sums(item, ib);

  //printf("%g %g %g %g\n", xsm->s, xsm->se, xsm->se2, xsm->se3);
  at_mb_sm__add(sm, ginvwf, e, do_2nd);

  /* we call normalization when the weight starts to blow up */
  if (item->amplifier > amplifier_threshold) {
    at_mb_accum_winaccum_item__normalize(item);
  }

  item->visits += 1;

}



void at_mb_accum_winaccum_item__init(
    at_mb_accum_winaccum_item_t *item,
    int i, at_mb_win_t *win)
{
  int m;
  int j;

  item->i = i;
  item->js = win->js_bin[i];
  item->jt = win->jt_bin[i];

  m = item->jt - item->js;
  if ((item->sums = (at_mb_sm_t *) calloc(m, sizeof(at_mb_sm_t))) == NULL) {
    fprintf(stderr, "Error@at.mb.accum.winaccum: no memory for winaccum item->sums\n");
    exit(1);
  }

  for (j = 0; j < m; j++) {
    at_mb_sm__init(item->sums + j);
  }

  item->amplifier = 1.0;
  item->visits = 0.0;
}



void at_mb_accum_winaccum_item__clear(
    at_mb_accum_winaccum_item_t *item)
{
  int j, m = item->jt - item->js;

  for (j = 0; j < m; j++) {
    at_mb_sm__clear(item->sums + j);
  }

  item->amplifier = 1.0;
  item->visits = 0.0;
}



void at_mb_accum_winaccum_item__finish(
    at_mb_accum_winaccum_item_t *item)
{
  free(item->sums);
}



#endif
