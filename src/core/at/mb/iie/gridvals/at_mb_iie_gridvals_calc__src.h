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

#ifndef AT_MB_IIE_GRIDVALS_CALC__SRC_H__
#define AT_MB_IIE_GRIDVALS_CALC__SRC_H__



/* integral-identity estimators for grid point values */



#include "at_mb_iie_gridvals_calc.h"

#include "../lr/at_mb_iie_lr.h"
#include "../et/at_mb_iie_et.h"
#include "../../win/at_mb_win.h"
#include "../../../distr/at_distr.h"


/* set quality bit */
void at_mb_iie__set_quality_bit(unsigned *ptr, unsigned mask, int on)
{
  if (mask) {
    if (on) {
      *ptr |= mask;
    } else {
      *ptr &= ~mask;
    }
  }
}




void at_mb_iie_gridvals__calc_energy(at_mb_t *mb)
{
  int i;
  at_mb_iie_t *iie = mb->iie;
  at_mb_iie_lr_t *lr = mb->iie->lr;
  at_mb_iie_gridvals_t *gridvals = mb->iie->gridvals;

  for (i = 0; i <= iie->n; i++) {
    at_mb_iie_gridvals_item_t *item = gridvals->items + i;

    at_mb_iie_lr__init_instance(lr,
        MB_IIE_LR__TYPE_GRID,
        MB_IIE_LR__WIN_DIV_DEFAULT,
        i,
        mb->win->js_grid_res[i],
        mb->win->jt_grid_res[i]);

    at_mb_iie_lr__collect_moments(lr);

    item->e = at_mb_iie_lr__balance_moments(lr);

    at_mb_iie_gridvals_item__set_quality_bit(item,
        AT_MB_IIE_GRIDVALS__QUALITY_BIT_EHAT,
        lr->quality);

  }

}



/* collect second-order moments from the left and right windows
 *
 * This routine is similar to at_mb_iie_lr__collect_moments() but for second-order
 * moments and for grid-point values.
 * */
static void at_mb_iie_lr__collect_2nd_order_moments(at_mb_iie_lr_t *lr)
{
  double x;
  int j, jx, lr_id;
  double bdel = lr->mb->distr->domain->bdel;

  if (0 > lr->js || lr->js >= lr->jt || lr->jt > lr->mb->distr->domain->n) {
    fprintf(stderr, "bad window [%d, %d)", lr->js, lr->jt);
    exit(1);
  }

  for (j = lr->js; j < lr->jt; j++) { /* loop over bins */
    at_mb_sm_t *sm = lr->sm0 + j;

    if (sm->s < AT_MB_ACCUM_MIN_SIZE) {
      continue;
    }

    if (j < lr->jb) {
      jx = lr->js;
      lr_id = 0;
    } else {
      jx = lr->jt;
      lr_id = 1;
    }

    lr->s0[lr_id] += 1;
    lr->s1[lr_id] += sm->se2/sm->s;
    x = sm->se3/sm->s;

    lr->t1[lr_id] += x * (j - jx + 0.5);
  }

  lr->t1[0] *= -bdel;
  lr->t1[1] *= -bdel;
}



void at_mb_iie_gridvals__calc_cv(at_mb_t *mb)
{
  int i;
  double beta, e_var;
  at_mb_iie_lr_t *lr = mb->iie->lr;
  at_mb_iie_gridvals_t *gridvals = mb->iie->gridvals;

  if (!mb->need_cv) {
    return;
  }

  for (i = 0; i <= mb->distr->domain->n; i++) {
    at_mb_iie_gridvals_item_t *item = gridvals->items + i;

    at_mb_iie_lr__init_instance(lr,
        MB_IIE_LR__TYPE_GRID,
        MB_IIE_LR__WIN_DIV_DEFAULT,
        i,
        mb->win->js_grid_res[i],
        mb->win->jt_grid_res[i]);

    at_mb_iie_lr__collect_2nd_order_moments(lr);

    if (lr->t1[0] * lr->t1[1] > 0) {

      /* t1[0] and t1[1] share the same sign */

      double del = (lr->t1[0] + lr->t1[1]) / (lr->s1[0] + lr->s1[1]);
      if (del < -1) {
        del = -1;
      }

      if (fabs(del) > lr->cv_shift_max) {
        del = lr->cv_shift_max * ((del > 0) ? 1.0 : -1.0);
      }

      e_var = (lr->s1[0]+lr->s1[1]) * (1+del) / (lr->s0[0]+lr->s0[1]);

    } else {

      /* normal case */
      e_var = at_mb_iie_lr__balance_moments(lr);

    }

    beta = mb->distr->domain->barr[i];

    item->cv = mb->boltz * (beta * beta) * e_var;

    at_mb_iie_gridvals_item__set_quality_bit(item,
        AT_MB_IIE_GRIDVALS__QUALITY_BIT_CV,
        lr->quality);

  }

}


void at_mb_iie_gridvals__calc_lnz(at_mb_t *mb)
{
  int i;
  at_mb_iie_t *iie = mb->iie;
  at_mb_iie_gridvals_t *gridvals = mb->iie->gridvals;
  at_mb_iie_gridvals_item_t *item = gridvals->items;
  at_distr_domain_t *domain = mb->distr->domain;

  item->lnz = 0.0;
  at_mb_iie_gridvals_item__set_quality_bit(item, AT_MB_IIE_GRIDVALS__QUALITY_BIT_LNZ, 1);

  /* estimate the partition function */
  for (i = 0; i < iie->n; i++) {
    ++item;

    double et = at_mb_iie_et__calc_et(iie, i);

    item->lnz = gridvals->items[i].lnz + et * (domain->barr[i] - domain->barr[i+1]);

    at_mb_iie_gridvals_item__set_quality_bit(item, AT_MB_IIE_GRIDVALS__QUALITY_BIT_LNZ, 1);
  }
}


void at_mb_iie_gridvals__calc(at_mb_t *mb)
{
  at_mb_iie_gridvals__calc_energy(mb);

  at_mb_iie_gridvals__calc_cv(mb);

  at_mb_iie_gridvals__calc_lnz(mb);
}




#endif
