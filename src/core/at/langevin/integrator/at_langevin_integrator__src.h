#ifndef AT_LANGEVIN_INTEGRATOR__SRC_H__
#define AT_LANGEVIN_INTEGRATOR__SRC_H__



/* implementation dependent headers */
#include "../../mb/at_mb_basic.h"

#include "at_langevin_integrator.h"
#include "../zerofiller/at_langevin_zerofiller.h"


int at_langevin_integrator__init(at_langevin_integrator_t *intgr,
    at_mb_t *mb, int use_zerofiller)
{
  intgr->n = mb->n;
  intgr->beta_min = mb->bmin;
  intgr->beta_max = mb->bmax;
  intgr->beta_del = mb->bdel;

  intgr->use_zerofiller = use_zerofiller;

  intgr->accum = mb->accum;

  if (intgr->use_zerofiller) {
    at_langevin_zerofiller__init(intgr->zerofiller, mb);
    intgr->vals = NULL;
  } else {
    if ((intgr->vals = (double *) calloc(intgr->n, sizeof(double))) == NULL) {
      fprintf(stderr, "no memory for at->mb->intgr->vals, %d of double\n", intgr->n);
    }
  }

  return 0;
}



void at_langevin_integrator__finish(at_langevin_integrator_t *intgr)
{
  if (intgr->use_zerofiller) {
    at_langevin_zerofiller__finish(intgr->zerofiller);
  } else {
    free(intgr->vals);
  }
}



static int at_langevin_integrator__beta_to_index(at_langevin_integrator_t *intgr, double beta)
{
  return (int) ((beta - intgr->beta_min) / intgr->beta_del);
}



static void at_langevin_integrator__init_indicies(at_langevin_integrator_t *intgr)
{
  intgr->ib_begin = at_langevin_integrator__beta_to_index(intgr, intgr->beta_begin);

  intgr->ib_end = at_langevin_integrator__beta_to_index(intgr, intgr->beta_end);
}



static void at_langevin_integrator__init_integral(at_langevin_integrator_t *intgr,
    double beta_from, double beta_to)
{
  if (beta_from < beta_to) {
    intgr->beta_begin = beta_from;
    intgr->beta_end = beta_to;
    intgr->sgn = 1;
  } else {
    intgr->beta_begin = beta_to;
    intgr->beta_end = beta_from;
    intgr->sgn = -1;
  }

  intgr->dbeta_left = 0;
  intgr->dbeta_right = 0;

  // out of range cases

  // entirely on the left: begin <= end < min
  if (intgr->beta_end < intgr->beta_min) {
    // entirely out of range
    intgr->dbeta_left = intgr->beta_end - intgr->beta_begin;
    intgr->beta_begin = intgr->beta_min;
    intgr->beta_end = intgr->beta_min;
  }

  // entirely on the right: max <= begin <= end
  if (intgr->beta_begin >= intgr->beta_max) {
    intgr->dbeta_right = intgr->beta_end - intgr->beta_begin;
    intgr->beta_begin = intgr->beta_max;
    intgr->beta_end = intgr->beta_max;
  }

  // left margin: begin < min
  if (intgr->beta_begin < intgr->beta_min) {
    intgr->dbeta_left = intgr->beta_min - intgr->beta_begin;
    intgr->beta_begin = intgr->beta_min;
  }

  // right margin: max < end
  if (intgr->beta_end >= intgr->beta_max) {
    intgr->dbeta_right = intgr->beta_end - intgr->beta_max;
    intgr->beta_end = intgr->beta_max - 1e-15 * intgr->beta_del;
  }

  at_langevin_integrator__init_indicies(intgr);

}



static double *at_langevin_integrator__fill_range_with_proper_sums_plain(
    at_langevin_integrator_t *intgr,
    int ib_begin, int ib_end)
{
  int ib;
  at_mb_accum_t *accum = intgr->accum;
  at_mb_sm_t *sm;

  if (accum->use_winaccum) {

    for (ib = ib_begin; ib <= ib_end; ib++) {
      sm = at_mb_accum_winaccum_item__get_sums(accum->winaccum->items + ib, ib);
      intgr->vals[ib] = at_mb_sm__get_mean(sm, AT_MB_ACCUM_MIN_SIZE);
    }

  } else {

    // this loop is deliberately separated from the one above
    // for better looping efficiency
    for (ib = ib_begin; ib <= ib_end; ib++) {
      sm = accum->sums + ib;
      intgr->vals[ib] = at_mb_sm__get_mean(sm, AT_MB_ACCUM_MIN_SIZE);
    }

  }

  return intgr->vals;
}



double at_langevin_integrator__get_unsigned_integral(at_langevin_integrator_t *intgr,
    at_langevin_zerofiller_t *zf)
{
  int ib;
  int ib_begin = intgr->ib_begin;
  int ib_end = intgr->ib_end;
  double ival = 0.0;
  double *vals;

  if (intgr->use_zerofiller) {
    vals = at_langevin_zerofiller__fill_range_with_proper_sums(
        zf, ib_begin, ib_end);
  } else {
    // use raw values, more efficient but may not
    // handle missing values correctly
    vals = at_langevin_integrator__fill_range_with_proper_sums_plain(
        intgr, ib_begin, ib_end);
  }

  if (ib_begin < ib_end) {

    double dbeta_first_bin = (intgr->beta_min + (ib_begin+1) * intgr->beta_del) - intgr->beta_begin;
    ival = vals[ib_begin] * dbeta_first_bin;

    double dbeta_last_bin = intgr->beta_end - (intgr->beta_min + ib_end * intgr->beta_del);
    ival += vals[ib_end] * dbeta_last_bin;

    for (ib = ib_begin + 1; ib < ib_end; ib++) {
      ival += vals[ib] * intgr->beta_del;
    }

  } else {

    // ib_begin == ib_end
    //
    // both beta values lie in the same bin

    ival = vals[ib_begin] * (intgr->beta_end - intgr->beta_begin);

  }

  // add the out-of-range marginal integral values
  if (intgr->dbeta_left > 0) {
    ival += vals[ib_begin] * intgr->dbeta_left;
  }

  if (intgr->dbeta_right > 0) {
    ival += vals[ib_end] * intgr->dbeta_right;
  }

  return ival;
}



double at_langevin_integrator__integrate(at_langevin_integrator_t *intgr,
    double beta_from, double beta_to)
{
  at_langevin_integrator__init_integral(intgr, beta_from, beta_to);

  return intgr->sgn * at_langevin_integrator__get_unsigned_integral(
      intgr, intgr->zerofiller);
}



#endif
