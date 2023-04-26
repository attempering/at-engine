#ifndef AT__MB_IIE_GRIDVALS_CALC__SRC_H__
#define AT__MB_IIE_GRIDVALS_CALC__SRC_H__



/* integral-identity estimators for grid point values */



#include "mb_iie_gridvals_calc.h"

#include "../lr/mb_iie_lr.h"
#include "../et/mb_iie_et.h"
#include "../../win/mb_win.h"



/* set quality bit */
void mb_iie__set_quality_bit(unsigned *ptr, unsigned mask, int on)
{
  if (mask) {
    if (on) {
      *ptr |= mask;
    } else {
      *ptr &= ~mask;
    }
  }
}




void mb_iie_gridvals__calc_energy(mb_t *mb)
{
  int i;
  mb_iie_t *iie = mb->iie;
  mb_iie_lr_t *lr = mb->iie->lr;
  mb_iie_gridvals_t *gridvals = mb->iie->gridvals;

  for (i = 0; i <= iie->n; i++) {
    mb_iie_gridvals_item_t *item = gridvals->items + i;

    mb_iie_lr__init_instance(lr,
        MB_IIE_LR__TYPE_GRID,
        MB_IIE_LR__WIN_DIV_DEFAULT,
        i,
        mb->win->js_grid_res[i],
        mb->win->jt_grid_res[i]);

    mb_iie_lr__collect_moments(lr);

    item->e = mb_iie_lr__balance_moments(lr);

    mb_iie_gridvals_item__set_quality_bit(item,
        MB_IIE_GRIDVALS__QUALITY_BIT_EHAT,
        lr->quality);

  }

}



/* collect second-order moments from the left and right windows
 *
 * This routine is similar to mb_iie_lr__collect_moments() but for second-order
 * moments and for grid-point values.
 * */
static void mb_iie_lr__collect_2nd_order_moments(mb_iie_lr_t *lr)
{
  double x;
  int j, jx, lr_id;
  mb_t *mb = lr->mb;

  if (0 > lr->js || lr->js >= lr->jt || lr->jt > lr->mb->n) {
    fprintf(stderr, "bad window [%d, %d)", lr->js, lr->jt);
    exit(1);
  }

  for (j = lr->js; j < lr->jt; j++) { /* loop over bins */
    sm_t *sm = lr->sm0 + j;

    if (sm->s < MB_ACCUM_MIN_SIZE) {
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

  lr->t1[0] *= -mb->bdel;
  lr->t1[1] *= -mb->bdel;
}



void mb_iie_gridvals__calc_cv(mb_t *mb)
{
  int i, need_cv;
  double beta, e_var;
  mb_iie_lr_t *lr = mb->iie->lr;
  mb_iie_gridvals_t *gridvals = mb->iie->gridvals;

  need_cv = (mb->flags & MB_CV);

  if (!need_cv) {
    return;
  }

  for (i = 0; i <= mb->n; i++) {
    mb_iie_gridvals_item_t *item = gridvals->items + i;

    mb_iie_lr__init_instance(lr,
        MB_IIE_LR__TYPE_GRID,
        MB_IIE_LR__WIN_DIV_DEFAULT,
        i,
        mb->win->js_grid_res[i],
        mb->win->jt_grid_res[i]);

    mb_iie_lr__collect_2nd_order_moments(lr);

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
      e_var = mb_iie_lr__balance_moments(lr);

    }

    beta = mb->barr[i];

    item->cv = mb->boltz * (beta * beta) * e_var;

    mb_iie_gridvals_item__set_quality_bit(item,
        MB_IIE_GRIDVALS__QUALITY_BIT_CV,
        lr->quality);

  }

}


void mb_iie_gridvals__calc_lnz(mb_t *mb)
{
  int i;
  mb_iie_t *iie = mb->iie;
  mb_iie_gridvals_t *gridvals = mb->iie->gridvals;
  mb_iie_gridvals_item_t *item = gridvals->items;

  item->lnz = 0.0;
  mb_iie_gridvals_item__set_quality_bit(item, MB_IIE_GRIDVALS__QUALITY_BIT_LNZ, 1);

  /* estimate the partition function */
  for (i = 0; i < iie->n; i++) {
    ++item;

    double et = mb_iie_et__calc_et(iie, i);

    item->lnz = gridvals->items[i].lnz + et * (mb->barr[i] - mb->barr[i+1]);

    mb_iie_gridvals_item__set_quality_bit(item, MB_IIE_GRIDVALS__QUALITY_BIT_LNZ, 1);
  }
}


void mb_iie_gridvals__calc(mb_t *mb)
{
  mb_iie_gridvals__calc_energy(mb);

  mb_iie_gridvals__calc_cv(mb);

  mb_iie_gridvals__calc_lnz(mb);
}




#endif
