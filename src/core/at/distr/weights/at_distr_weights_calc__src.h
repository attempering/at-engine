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

#ifndef AT_DISTR_WEIGHTS_CALC__SRC_H__
#define AT_DISTR_WEIGHTS_CALC__SRC_H__


#include "at_distr_weights.h"
#include "../domain/at_distr_domain.h"

#include "composite/at_distr_weights_composite__src.h"
#include "at_distr_weights_order__src.h"



void at_distr_weights__init_ens_exp(at_distr_weights_t *w)
{
  const double eps = 1e-5;

  int ifac = (int) (w->ens_exp + 0.5); /* round to nearest int */

  if (fabs(w->ens_exp - ifac) < eps && ifac >= 0) {
    w->ens_exp_is_int = AT__TRUE;
    w->ens_exp_int = ifac;
  } else {
    w->ens_exp_is_int = AT__FALSE;
    w->ens_exp_int = 0;
  }
}



void at_distr_weights__init_ens_w(
    at_distr_weights_t *w,
    at_distr_domain_t *domain)
{
  int i;

  /* ens_w: array of ensemble weights at bin boundaries */
  at_utils__new_arr(w->ens_w, w->n + 1, double);
  for (i = 0; i <= w->n; i++) {
    double invw = at_distr_weights__calc_inv_weight_bounded(
        w, domain->barr[i], NULL, NULL, NULL);
    w->ens_w[i] = 1.0/invw;
  }
}



/* compute the ensemble f factor
 * f: f(beta)
 * *p_neg_df_dbeta: -d f(beta) / d(beta)
 * 
 * (deprecated) as the f value is not bounded
 * and can be very small, future application should prefer
 *
 *  at_distr_weights__calc_f_factor_bounded()
 *
 */
static double at_distr_weights__calc_f_factor_simple(
    const at_distr_weights_t *w,
    double beta, double *neg_dlnf_dbeta)
{
  double f = 1.0,
         neg_dlnf_dbeta_local = 0.0;

  if (w->mode == AT_DISTR_WEIGHTS_MODE__FLAT)
  {
    f = 1.0;
    neg_dlnf_dbeta_local = 0.0;
  }
  else if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPOSITE)
  {
    f = at_distr_weights_composite__calc_f_factor_simple(
        w->composite, beta, &neg_dlnf_dbeta_local,
        ((at_distr_weights_t *) w)->log);
  }
  else if (w->mode == AT_DISTR_WEIGHTS_MODE__GAUSSIAN)
  {
    double del_beta = beta - w->beta0;
    f = exp(-0.5 * (del_beta * del_beta) * w->inv_sigma2);
    neg_dlnf_dbeta_local = del_beta * w->inv_sigma2;
  }
  else if (w->mode == AT_DISTR_WEIGHTS_MODE__EXPONENTIAL)
  {
    f = exp(-beta * w->c);
    neg_dlnf_dbeta_local = w->c;
  }
  else
  {
    at_utils_log__fatal(((at_distr_weights_t *) w)->log,
        "unknown mode %d\n",
        w->mode);
  }

  if (neg_dlnf_dbeta != NULL) {
    *neg_dlnf_dbeta = neg_dlnf_dbeta_local;
  }

  return f;
}



/* compute the ensemble f factor and its logarithmic derivative
 * f: f(beta)
 * *p_neg_dlnf_dbeta: -d log[f(beta)] / d(beta)
 */
static double at_distr_weights__calc_f_factor_bounded(
    const at_distr_weights_t *w,
    double beta,
    double *neg_dlnf_dbeta)
{
  zcom_xdouble_t f_;
  double f = 1.0;
  double neg_dlnf_dbeta_local = 0.0;

  if (w->mode == AT_DISTR_WEIGHTS_MODE__FLAT)
  {
    f = 1.0;
    neg_dlnf_dbeta_local = 0.0;
  }
  else
  {

    if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPOSITE)
    {
      f_ = at_distr_weights_composite__calc_f_factor_unbounded(
          w->composite, beta, &neg_dlnf_dbeta_local,
          ((at_distr_weights_t *) w)->log);
    }
    else if (w->mode == AT_DISTR_WEIGHTS_MODE__GAUSSIAN)
    {
      double del_beta = beta - w->beta0;
      f_ = zcom_xdouble__exp(-0.5 * (del_beta * del_beta) * w->inv_sigma2);
      neg_dlnf_dbeta_local = del_beta * w->inv_sigma2;
    }
    else if (w->mode == AT_DISTR_WEIGHTS_MODE__EXPONENTIAL)
    {
      f_ = zcom_xdouble__exp(-beta * w->c);
      neg_dlnf_dbeta_local = w->c;
    }
    else
    {
      f_ = zcom_xdouble__from_double(1.0);

      at_utils_log__fatal(((at_distr_weights_t *) w)->log,
          "unknown mode %d\n",
          w->mode);
    }

    f_ = zcom_xdouble__clamp_double(f_, w->f_min, w->f_max);
    f = zcom_xdouble__to_double(f_);

  }

  if (neg_dlnf_dbeta != NULL) {
    *neg_dlnf_dbeta = neg_dlnf_dbeta_local;
  }

  return f;
}



static double at_distr_weights__calc_invw_factor(
    const at_distr_weights_t *w, double beta)
{
  double beta_r;
  double invw;

  if (w->beta_max <= 0) {
    at_utils_log__fatal((at_utils_log_t *) (w->log),
        "invalid beta_max %g\n", w->beta_max);
  }

  beta_r = beta / w->beta_max; /* to relative beta */

  //fprintf(stderr, "beta_r %g, ens_exp %g, ens_exp_is_int %d, ens_exp_int %d\n",
  //  beta_r, w->ens_exp, w->ens_exp_is_int, w->ens_exp_int);

  if (w->ens_exp_is_int) {

    int ifac;

    for (invw = 1.0, ifac = 0; ifac < w->ens_exp_int; ifac++) {
      invw *= beta_r;
    }

  } else { /* unable to avoid exponential */

    invw = pow(beta_r, w->ens_exp);

  }

  return invw;
}


static double at_distr_weights__clamp_invwf(
    const at_distr_weights_t *w, double invwf)
{
  if (invwf > w->invwf_max) {

    invwf = w->invwf_max;

  } else if (invwf < w->invwf_min) {

    invwf = w->invwf_min;

  }

  return invwf;
}



double at_distr_weights__calc_inv_weight_simple(
    const at_distr_weights_t *w, double beta,
    double *neg_dlnwf_dbeta,
    double *f, double *neg_dlnf_dbeta)
{
  double f_local, invw, invwf, neg_dlnf_dbeta_local = 0.0;

  if (neg_dlnf_dbeta == NULL) {
    neg_dlnf_dbeta = &neg_dlnf_dbeta_local;
  }

  f_local = at_distr_weights__calc_f_factor_simple(
      w, beta, neg_dlnf_dbeta);

  if (f_local > w->f_max) {

    at_utils_log__warning (((at_distr_weights_t *) w)->log,
        "f overflows at %g, mode %d, beta=%g/%g, exp %g (%d, %d)\n",
        f_local, w->mode, beta, w->beta_max,
        w->ens_exp, w->ens_exp_int, w->ens_exp_is_int);

    f_local = w->f_max;

  } else if (f_local < w->f_min) {

    at_utils_log__warning (((at_distr_weights_t *) w)->log,
        "f underflows at %g, mode %d, beta=%g/%g, exp %g (%d, %d)\n",
        f_local, w->mode, beta, w->beta_max,
        w->ens_exp, w->ens_exp_int, w->ens_exp_is_int);

    f_local = w->f_min;

  }

  if (f != NULL) {
    *f = f_local;
  }

  // beta^{-ens_exp}
  invw = at_distr_weights__calc_invw_factor(w, beta);

  invwf = invw / f_local;

  invwf = at_distr_weights__clamp_invwf(w, invwf);
  //fprintf(stderr, "calc_inv_weight_simple(): beta %g, invw %g, f %g, %g\n", beta, invw, f_local, *neg_dlnf_dbeta);
  //getchar();

  if (neg_dlnwf_dbeta != NULL) {
    *neg_dlnwf_dbeta = (w->ens_exp / beta)
                     + *neg_dlnf_dbeta;
  }

  return invwf;
}


double at_distr_weights__calc_inv_weight_bounded(
    const at_distr_weights_t *w,
    double beta,
    double *neg_dlnwf_dbeta,
    double *f,
    double *neg_dlnf_dbeta)
{
  double f_local;
  double invwf;
  double invw;
  double neg_dlnf_dbeta_local = 0.0;

  if (neg_dlnf_dbeta == NULL) {
    neg_dlnf_dbeta = &neg_dlnf_dbeta_local;
  }

  f_local = at_distr_weights__calc_f_factor_bounded(
      w, beta, neg_dlnf_dbeta);

  if (f != NULL) {
    *f = f_local;
  }

  // beta^{-ens_exp}
  invw = at_distr_weights__calc_invw_factor(w, beta);

  // 1/(wf) = (1/w) / f
  invwf = invw / f_local;

  invwf = at_distr_weights__clamp_invwf(w, invwf);

  //fprintf(stderr, "calc_inv_weight_bounded(): beta %g, invw %g, f %g, %g\n", beta, invw, f_local, *neg_dlnf_dbeta);
  //getchar();

  if (neg_dlnwf_dbeta != NULL) {
    *neg_dlnwf_dbeta = (w->ens_exp / beta)
                     + *neg_dlnf_dbeta;
  }

  return invwf;
}


#endif
