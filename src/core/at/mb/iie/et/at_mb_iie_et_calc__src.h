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

#ifndef AT_MB_IIE_ET_CALC__SRC_H__
#define AT_MB_IIE_ET_CALC__SRC_H__



/* estimator of the average energy based on an integral identity */



#include "../at_mb_iie.h"
#include "at_mb_iie_et.h"

#include "../../accum/at_mb_accum.h"
#include "../../win/at_mb_win.h"
#include "../../../../zcom/zcom.h"




/* compute the estimated energy et of the given ib
 * using the single-bin estimator */
static double at_mb_iie_et__calc_et_single_bin(at_mb_iie_t *iie, int ib)
{
  at_mb_sm_t *sm;

  /* choose windowed accumulator sums or plain sums */
  sm = at_mb_accum__get_proper_sums(iie->accum, ib, ib);

  return at_mb_sm__get_mean(sm, AT_MB_ACCUM_MIN_SIZE);
}



/* update the estimated energy et of the given ib
 * using a linear combination of data from the left and right windows
 * */
static double at_mb_iie_et__calc_et_iie_lr(at_mb_iie_t *iie, int ib, int win_div)
{
  int js, jt;
  at_mb_iie_lr_t *lr = iie->lr;

  //fprintf(stderr, "lr %p, %s:%d\n", lr, __FILE__, __LINE__);

  js = iie->win->js_bin[ib];
  jt = iie->win->jt_bin[ib];

  zcom_util__exit_if (js < 0 || js >= jt || jt > iie->n,
      "invalid indices: js %d, jt %d, ib = %d/%d", js, jt, ib, iie->n);

  //fprintf(stderr, "ib %d, js %d, jt %d; %s:%d\n", ib, js, jt, __FILE__, __LINE__);

  // handle the degenerative cases:
  // window contains a single bin only,
  // or the caller explicitly requests the single-bin version
  if (iie->use_single_bin || jt == js + 1) {
    double et = at_mb_iie_et__calc_et_single_bin(iie, ib);

#if 0
    fprintf(stderr, "using single-bin estimator %d, ib %d, js %d, jt %d; et %g; %s:%d\n",
        iie->use_single_bin, ib, js, jt, et, __FILE__, __LINE__);
    if (iie->accum->winaccum->enabled) {
      at_mb_accum_winaccum_item_t *wi;
      at_mb_sm_t *sm0 = at_mb_accum__get_proper_sums0_and_winaccum_item(iie->accum, ib, &wi);
      int j;
      for (j = wi->js; j < wi->jt; j++) {
        fprintf(stderr, "  %d %g %g\n", j, sm0[j].s, at_mb_sm__get_mean(sm0+j, 1e-6));
      }
    }
    if (et == 0.0) exit(1);
#endif

    if (et != 0.0) {
      lr->success = 1;
      lr->quality = 1;
    } else {
      lr->success = 0;
      lr->quality = 0;
    }

    return et;
  }

  at_mb_iie_lr__init_instance(lr, MB_IIE_LR__TYPE_BIN, win_div,
      ib, js, jt);

  /* collect moments from the left & right windows */
  at_mb_iie_lr__collect_moments(lr);

  //fprintf(stderr, "ib %d, js %d, jt %d, %s:%d\n", ib, js, jt, __FILE__, __LINE__);
  //fprintf(stderr, "s0 %g, %g, %s:%d\n", lr->s0[0], lr->s0[1], __FILE__, __LINE__);

  /* compute the coefficients of linear combination and Et */
  return at_mb_iie_lr__balance_moments(lr);
}



double at_mb_iie_et__calc_et(at_mb_iie_t *iie, int ib)
{
  double et_val;
  at_mb_iie_lr_t *lr = iie->lr;
  at_mb_iie_et_t *et = iie->et;
  at_mb_iie_et_item_t *item = et->items + ib;

  zcom_util__exit_if (ib < 0 || ib >= iie->n,
      "bad ib %d [0, %d).\n", ib, iie->n);

  et_val = at_mb_iie_et__calc_et_iie_lr(iie, ib, MB_IIE_LR__WIN_DIV_DEFAULT);

  item->value = et_val;
  item->imbalance = lr->imbalance;
  item->quality = lr->success && lr->quality;

  //static long count = 0;
  //if (++count % 1000000 == 0) {
    //fprintf(stderr, "__calc_et() called %ld times\n", count);
  //}

  return et_val;
}



#endif
