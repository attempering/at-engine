#ifndef AT__MB_ACCUM_BASIC__SRC_H__
#define AT__MB_ACCUM_BASIC__SRC_H__


#include "mb_accum.h"

#include "winaccum/mb_accum_winaccum.h"

#include "../../sm/sm.h"

#include "../../zcom/util/util.h"
#include "../../zcom/cfg/cfg.h"
#include "../../zcom/endn/endn.h"


/* return the origin of sums for the window estimator at win_id */
static sm_t *mb_accum__get_sums0(mb_accum_t *accum)
{
  return accum->sums;
}


/* return the address of sums for the window estimator at win_id */
sm_t *mb_accum__get_sums(mb_accum_t *accum, int j)
{
  return accum->sums + j;
}



sm_t *mb_accum__get_proper_sums0_and_winaccum_item(
    mb_accum_t *accum, int win_id,
    mb_accum_winaccum_item_t **winaccum_item)
{

  if (accum->use_winaccum) {

    if (win_id == accum->n) {
      win_id = accum->n - 1;
    }

    *winaccum_item = accum->winaccum->items + win_id;

    return (*winaccum_item)->sums - (*winaccum_item)->js;

  } else {

    *winaccum_item = NULL;

    return mb_accum__get_sums0(accum);

  }

}



sm_t *mb_accum__get_proper_sums(mb_accum_t *accum, int win_id, int ib)
{
  mb_accum_winaccum_item_t *winaccum_item;

  sm_t *sm0 = mb_accum__get_proper_sums0_and_winaccum_item(
      accum, win_id, &winaccum_item);

  return sm0 + ib;
}



double mb_accum__get_window_visits(mb_accum_t *accum, int win_id)
{
  if (accum->use_winaccum) {
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



void mb_accum__add(mb_accum_t *accum, int ib, double invwf, double e, int do_2nd,
    mb_shk_t *shk, double total_visits)
{
  // for the plain accumulators
  sm_t *sm = mb_accum__get_sums(accum, ib);
  //printf("%g %g %g %g\n", sm->s, sm->se, sm->se2, sm->se3);
  sm__add(sm, invwf, e, do_2nd);

  // for the window accumulators
  if (accum->use_winaccum) { /* add to damping data */
    mb_accum_winaccum__add(
        accum->winaccum, ib,
        invwf, e, do_2nd, shk, total_visits);
  }

}


void mb_accum__calc_win_total(mb_accum_t *accum)
{
  if (accum->use_winaccum) {
    mb_accum_winaccum__calc_win_total(accum->winaccum, accum->win_total);
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



void mb_accum__init(mb_accum_t *accum, int n, mb_win_t *win, unsigned flags)
{
  int i;

  accum->n = n;

  accum->win = win;

  accum->flags = flags;

  /* sums: normal data */
  exit_if ((accum->sums = (sm_t *) calloc((accum->n + 1), sizeof(sm_t))) == NULL,
      "no memory! var: accum->sums, type: sm_t\n");

  for (i = 0; i < accum->n; i++) {
    sm__init(accum->sums+i);
  }

  accum->use_winaccum = flags & MB_USE_WIN_ACCUM;

  /* win_total: total of sum.s over a multiple-bin temperature window */
  exit_if ((accum->win_total = (double *) calloc(n, sizeof(double))) == NULL,
    "no memory! var: accum->win_total, type: double\n");

  for (i = 0; i < n; i++) {
    accum->win_total[i] = 0.0;
  }

}


void mb_accum__clear(mb_accum_t *accum)
{
  int i;

  if (accum->sums != NULL) {
    for (i = 0; i < accum->n; i++) {
      sm__clear(accum->sums + i);
    }
  }

  for (i = 0; i < accum->n; i++) {
    accum->win_total[i] = 0.0;
  }

  mb_accum_winaccum__clear(accum->winaccum);

}



void mb_accum__finish(mb_accum_t *accum)
{
  free(accum->sums);

  free(accum->win_total);

  if (accum->use_winaccum) {
    //fprintf(stderr, "mb_accum__finish() %p %p\n", accum, accum->winaccum); getchar();
    mb_accum_winaccum__finish(accum->winaccum);
  }
}



#endif