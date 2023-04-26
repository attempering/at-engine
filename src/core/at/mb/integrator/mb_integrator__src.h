#ifndef AT__MB_INTEGRATOR__SRC_H__
#define AT__MB_INTEGRATOR__SRC_H__



/* implementation dependent headers */
#include "../mb_basic.h"

#include "mb_integrator.h"


int at_mb_integrator__init(at_mb_integrator_t *intgr, at_mb_t *mb)
{
  intgr->n = mb->n;
  intgr->beta_min = mb->bmin;
  intgr->beta_max = mb->bmax;
  intgr->beta_del = mb->bdel;

  // save a reference to the zerofiller
  intgr->zerofiller = mb->zerofiller;

  return 0;
}


void at_mb_integrator__finish(at_mb_integrator_t *intgr)
{
}


static int mb_integrator_beta_to_index(at_mb_integrator_t *intgr, double beta)
{
  return (int) ((beta - intgr->beta_min) / intgr->beta_del);
}

static void mb_integrator_init_indicies(at_mb_integrator_t *intgr)
{
  intgr->ib_begin = mb_integrator_beta_to_index(intgr, intgr->beta_begin);

  intgr->ib_end = mb_integrator_beta_to_index(intgr, intgr->beta_end);
}



static void mb_integrator_init_integral(at_mb_integrator_t *intgr, double beta_from, double beta_to)
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

  mb_integrator_init_indicies(intgr);

}


double mb_integrator_get_unsigned_integral(at_mb_integrator_t *intgr,
    at_mb_zerofiller_t *zf)
{
  int ib;
  int ib_begin = intgr->ib_begin;
  int ib_end = intgr->ib_end;
  double ival = 0.0;

  mb_zerofiller_fill_range_with_proper_sums(zf, ib_begin, ib_end);

  if (ib_begin < ib_end) {

    double dbeta_first_bin = (intgr->beta_min + (ib_begin+1) * intgr->beta_del) - intgr->beta_begin;
    ival = zf->vals[ib_begin] * dbeta_first_bin;

    double dbeta_last_bin = intgr->beta_end - (intgr->beta_min + ib_end * intgr->beta_del);
    ival += zf->vals[ib_end] * dbeta_last_bin;

    for (ib = ib_begin + 1; ib < ib_end; ib++) {
      ival += zf->vals[ib] * intgr->beta_del;
    }

  } else {

    // ib_begin == ib_end
    //
    // both beta values lie in the same bin

    ival = zf->vals[ib_begin] * (intgr->beta_end - intgr->beta_begin);

  }

  // add the out-of-range marginal integral values
  if (intgr->dbeta_left > 0) {
    ival += zf->vals[ib_begin] * intgr->dbeta_left;
  }

  if (intgr->dbeta_right > 0) {
    ival += zf->vals[ib_end] * intgr->dbeta_right;
  }

  return ival;
}



double at_mb_integrator__integrate(at_mb_integrator_t *intgr, double beta_from, double beta_to)
{
  mb_integrator_init_integral(intgr, beta_from, beta_to);

  return intgr->sgn * mb_integrator_get_unsigned_integral(intgr, intgr->zerofiller);
}



#endif
