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

#ifndef AT_DISTR_WEIGHTS__SRC_H__
#define AT_DISTR_WEIGHTS__SRC_H__


#include "at_distr_weights.h"
#include "../domain/at_distr_domain.h"
#include "../../../zcom/zcom.h"

#include "components/at_distr_weights_components__src.h"



static void at_distr_weights__init_ens_exp(at_distr_weights_t *w)
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



int at_distr_weights__cfg_init(at_distr_weights_t *w,
    at_distr_domain_t *domain, zcom_cfg_t *cfg, at_bool_t verbose)
{
  int i;

  w->beta_min = domain->beta_min;
  w->beta_max = domain->beta_max;
  w->n = domain->n;

  /* ens_exp: ensemble exponent of beta */
  w->ens_exp = 1.0;
  if (0 != zcom_cfg__get(cfg, &w->ens_exp, "ensemble-factor,ensemble-exp,ensemble-exponent", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at.distr.weights: assuming default distr->weights->ens_exp = 1.0, key: ensemble-exponent\n");
  }

  at_distr_weights__init_ens_exp(w);

  /* flat ensemble mode */
  w->mode = 0;
  if (0 != zcom_cfg__get(cfg, &w->mode, "ensemble-mode", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.distr.weights: assuming default distr->weights->mode = 0, key: ensemble-mode\n");
  }

  /* default values */
  w->beta0 = 0.5 * (w->beta_min + w->beta_max);
  w->inv_sigma2 = 1.0;
  w->c = 0.0;

  if(w->mode == AT_DISTR_WEIGHTS_MODE__GAUSSIAN) {

    if (verbose) fprintf(stderr, "Info@at.distr.weights: single-Gaussian distribution mode\n");

    if (0 != zcom_cfg__get(cfg, &w->beta0, "ensemble-beta0", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.distr.weights: assuming default distr->weights->beta0 = 0.5 * (w->beta_max + w->beta_min), key: ensemble-beta0\n");
    }

    w->sigma = 1.0;
    if (0 != zcom_cfg__get(cfg, &w->sigma, "ensemble-sigma", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.distr.weights: assuming default distr->weights->sigma = 1.0, key: ensemble-sigma\n");
    }

    if (w->sigma <= 0) {
      fprintf(stderr, "Error@at.distr.weights: sigma %lf is not positive!\n", w->sigma);
      goto ERR;
    }

    w->inv_sigma2 = 1.0/w->sigma/w->sigma;

  } else if(w->mode == AT_DISTR_WEIGHTS_MODE__EXPONENTIAL) {

    if (verbose) fprintf(stderr, "Info@at.distr.weights: single-exponential distribution mode\n");

    w->c = 0.0;
    if (0 != zcom_cfg__get(cfg, &w->c, "ensemble-c", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.distr.weights: assuming default distr->weights->c = 0.0, key: ensemble-c\n");
    }

  } else if(w->mode == AT_DISTR_WEIGHTS_MODE__COMPONENTS) {

    at_distr_weights_components__cfg_init(w->components, domain, cfg, verbose);

  } else if(w->mode != 0) {

    fprintf(stderr, "invalid ensemble mode\n");
    goto ERR;

  }

  /* ens_w: array of ensemble weights at bin boundaries */
  if ((w->ens_w = (double *) calloc((w->n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "Error@at.distr.weights: no memory! var: w->ens_w, type: double\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i <= w->n; i++) {
    double invw = at_distr_weights__calc_inv_weight(w, domain->barr[i], NULL, NULL, NULL);
    w->ens_w[i] = 1.0/invw;
  }

  return 0;

ERR:
  return 1;
}


void at_distr_weights__manifest(const at_distr_weights_t *w, at_utils_manifest_t *manifest)
{

  at_utils_manifest__print_double(manifest, w->ens_exp, "distr->weights->ens_exp", "ensemble-factor");

  at_utils_manifest__print_int(manifest, w->mode, "distr->weights->mode", "ensemble-mode");

  if (w->mode == AT_DISTR_WEIGHTS_MODE__GAUSSIAN) {

    at_utils_manifest__print_double(manifest, w->beta0, "distr->weights->beta0", "ensemble-beta0");
    at_utils_manifest__print_double(manifest, w->sigma, "distr->weights->sigma", "ensemble-sigma");

  } else if (w->mode == AT_DISTR_WEIGHTS_MODE__EXPONENTIAL) {

    at_utils_manifest__print_double(manifest, w->c, "distr->weights->c", "ensemble-c");

  } else if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPONENTS) {

    at_distr_weights_components__manifest(w->components, manifest);

  }

  /* ens_w: array of ensemble weights at bin boundaries */
  at_utils_manifest__print_double_arr(manifest, w->ens_w, w->n, "distr->weights->ens_w");

}



void at_distr_weights__finish(at_distr_weights_t *w)
{
  if (w->ens_w != NULL) {
    free(w->ens_w);
  }

  if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPONENTS) {
    at_distr_weights_components__finish(w->components);
  }

}



/* compute the ensemble f factor
 * f: f(beta);
 * *p_neg_df_dbeta: -df(beta)/dbeta;
 */
static double at_distr_weights__calc_f_factor(const at_distr_weights_t *w,
    double beta, double *p_neg_df_dbeta)
{
  double f, neg_df_dbeta;

  /* invwf: 1/w(beta)/f(beta);
   * f: f(beta);
   * neg_df_dbeta: -df(beta)/dbeta;
   */

  if (w->mode == AT_DISTR_WEIGHTS_MODE__FLAT)
  {
    f = 1.0;
    neg_df_dbeta = 0.0;
  }
  else if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPONENTS)
  {
    f = at_distr_weights_components__calc_f_factor(
      w->components, beta, &neg_df_dbeta);
  }
  else if (w->mode == AT_DISTR_WEIGHTS_MODE__GAUSSIAN)
  {
    double del_beta = beta - w->beta0;
    f = exp(-0.5 * (del_beta * del_beta) * w->inv_sigma2);
    neg_df_dbeta = f * del_beta * w->inv_sigma2;
  }
  else if (w->mode == AT_DISTR_WEIGHTS_MODE__EXPONENTIAL)
  {
    f = exp(-beta * w->c);
    neg_df_dbeta = f * w->c;
  }
  else
  {
    fprintf(stderr, "Error@at.distr.weights: unknown mode %d\n",
        w->mode);
    exit(1);
  }

  if (p_neg_df_dbeta != NULL) {
    *p_neg_df_dbeta = neg_df_dbeta;
  }

  return f;
}


static double at_distr_weights__calc_invw_factor(const at_distr_weights_t *w, double beta)
{
  double beta_r = beta / w->beta_max; /* to relative beta */
  double invw;

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


double at_distr_weights__calc_inv_weight(const at_distr_weights_t *w, double beta,
    double *neg_dlnwf_dbeta, double *pf, double *neg_df_dbeta)
{
  double f, invw, invwf, neg_df_dbeta_ = 0.0;
  const double invwf_max = 1e6;
  const double invwf_min = 1e-6;

  if (neg_df_dbeta == NULL) {
    neg_df_dbeta = &neg_df_dbeta_;
  }

  f = at_distr_weights__calc_f_factor(w, beta, neg_df_dbeta);

  if (pf != NULL) {
    *pf = f;
  }

  // beta^{-ens_exp}
  invw = at_distr_weights__calc_invw_factor(w, beta);

  invwf = invw / f;

  //fprintf(stderr, "beta %g, invw %g, f %g\n", beta, invw, f);
  //getchar();

  zcom_utils__exit_if (invwf > invwf_max || invwf < invwf_min,
      "bad invwf=%g, f=%g, beta=%g\n", invwf, beta);

  if (neg_dlnwf_dbeta != NULL) {
    *neg_dlnwf_dbeta = (w->ens_exp / beta)
                     + (*neg_df_dbeta / f);
  }

  return invwf;
}


#endif
