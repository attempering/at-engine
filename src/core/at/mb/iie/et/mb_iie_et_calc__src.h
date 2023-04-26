#ifndef AT__MB_IIE_ET_CALC__SRC_H__
#define AT__MB_IIE_ET_CALC__SRC_H__



/* estimator of the average energy based on an integral identity */



#include "../mb_iie.h"
#include "mb_iie_et.h"

#include "../../accum/mb_accum.h"
#include "../../win/mb_win.h"
#include "../../../zcom/util/util.h"




/* compute the estimated energy et of the given ib
 * using the single-bin estimator */
static double at_mb_iie_et__calc_et_single_bin(at_mb_iie_t *iie, int ib)
{
  at_mb_sm_t *sm;

  /* choose windowed accumulator sums or plain sums */
  sm = at_mb_accum__get_proper_sums(iie->accum, ib, ib);

  return at_mb_sm__get_mean(sm, MB_ACCUM_MIN_SIZE);
}



/* update the estimated energy et of the given ib
 * using a linear combination of data from the left and right windows
 * */
static double at_mb_iie_et__calc_et_iie_lr(at_mb_iie_t *iie, int ib, int win_div)
{
  int js, jt;
  at_mb_iie_lr_t *lr = iie->lr;

  js = iie->win->js_bin[ib];
  jt = iie->win->jt_bin[ib];

  zcom_util__exit_if (js < 0 || js >= jt || jt > iie->n,
      "invalid indices: js %d, jt %d, ib = %d/%d", js, jt, ib, iie->n);

  // handle the degenerative cases:
  // window contains a single bin only,
  // or the caller explicitly requests the single-bin version
  if ((iie->flags & MB_SINGLE_BIN) || jt == js + 1) {
    return at_mb_iie_et__calc_et_single_bin(iie, ib);
  }

  at_mb_iie_lr__init_instance(lr, MB_IIE_LR__TYPE_BIN, win_div,
      ib, js, jt);

  /* collect moments from the left & right windows */
  at_mb_iie_lr__collect_moments(lr);

#ifdef MB_DBG__
  fprintf(stderr, "ib %d, jb %d, js %d, jt %d; t1 %g %g, tb %g\n", ib, jb, js, jt, t1[0], t1[1], tb);
  fprintf(stderr, "s0 %g %g; s1/s0 %g %g\n", s0[0], s0[1], s1[0]/s0[0], s1[1]/s0[1]);
#endif

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

  return et_val;
}



#endif
