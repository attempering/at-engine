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

#include <stdlib.h>

#include "../../../zcom/zcom.h"



int at_distr_weights__cfg_init(at_distr_weights_t *w,
    at_distr_domain_t *domain, zcom_cfg_t *cfg, at_bool_t verbose)
{
  int i;

  w->beta_min = domain->beta_min;
  w->beta_max = domain->beta_max;
  w->n = domain->n;

  /* ens_exp: ensemble exponent of beta */
  w->ens_exp = 1.0;
  if (0 != zcom_cfg__get(cfg, &w->ens_exp, "ensemble-factor", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default distr->weights->ens_exp = 1.0, key: ensemble-factor\n");
  }

  /* flat ensemble mode */
  w->mode = 0;
  if (0 != zcom_cfg__get(cfg, &w->mode, "ensemble-mode", "%d")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default distr->weights->mode = 0, key: ensemble-mode\n");
  }

  /* default values */
  w->beta0 = 0.5 * (w->beta_min + w->beta_max);
  w->inv_sigma2 = 1.0;
  w->c = 0.0;

  if(w->mode == 1)
  {
    if (0 != zcom_cfg__get(cfg, &w->beta0, "ensemble-beta0", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at: assuming default distr->weights->beta0 = 0.5 * (w->beta_max + w->beta_min), key: ensemble-beta0\n");
    }

    w->sigma = 1.0;
    if (0 != zcom_cfg__get(cfg, &w->sigma, "ensemble-sigma", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at: assuming default distr->weights->sigma = 1.0, key: ensemble-sigma\n");
    }

    if (w->sigma <= 0) {
      fprintf(stderr, "ERROR: sigma %lf is not positive!\n", w->sigma);
      goto ERR;
    }

    w->inv_sigma2 = 1.0/w->sigma/w->sigma;

  }
  else if(w->mode == 2)
  {
    w->c = 0.0;
    if (0 != zcom_cfg__get(cfg, &w->c, "ensemble-c", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at: assuming default distr->weights->c = 0.0, key: ensemble-c\n");
    }
  }
  else if(w->mode != 0)
  {
    fprintf(stderr, "invalid ensemble mode\n");
    goto ERR;
  }

  /* ens_w: array of ensemble weights at bin boundaries */
  if ((w->ens_w = (double *) calloc((w->n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "at->error: no memory! var: w->ens_w, type: double\n");
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

  if (w->mode == 1) {
    at_utils_manifest__print_double(manifest, w->beta0, "distr->weights->beta0", "ensemble-beta0");
    at_utils_manifest__print_double(manifest, w->sigma, "distr->weights->sigma", "ensemble-sigma");
  } else if (w->mode == 2) {
    at_utils_manifest__print_double(manifest, w->c, "distr->weights->c", "ensemble-c");
  }

  /* ens_w: array of ensemble weights at bin boundaries */
  at_utils_manifest__print_double_arr(manifest, w->ens_w, w->n, "distr->weights->ens_w");

}



void at_distr_weights__finish(at_distr_weights_t *w)
{
  if (w->ens_w != NULL) {
    free(w->ens_w);
  }
}



/* compute the ensemble f factor
 * f: f(beta);
 * *pndfdbeta: -df(beta)/dbeta;
 */
static double at_distr_weights__calc_f_factor(const at_distr_weights_t *w, double beta, double *pndfdbeta)
{
  double f, ndfdbeta;

  /* invwf: 1/w(beta)/f(beta);
   * f: f(beta);
   * ndfdbeta: -df(beta)/dbeta;
   */

  if (w->mode == 1)
  {
    double beta_diff = beta - w->beta0;
    f = exp(-0.5 * (beta_diff * beta_diff) * w->inv_sigma2);
    ndfdbeta = f * beta_diff * w->inv_sigma2;
  }
  else if (w->mode == 2)
  {
    f = exp(-beta * w->c);
    ndfdbeta = f * w->c;
  }
  else
  {
    f = 1.0;
    ndfdbeta = 0.0;
  }

  if (pndfdbeta != NULL) {
    *pndfdbeta = ndfdbeta;
  }

  return f;
}


static double at_distr_weights__calc_invw_factor(const at_distr_weights_t *w, double beta)
{
  const double eps = 1e-5;
  double beta_r = beta / w->beta_max; /* to relative beta */

  int ifac = (int) (w->ens_exp + 0.5); /* round to nearest int */

  double invw;

  if (fabs(w->ens_exp - ifac) < eps && ifac >= 0) {
    for (invw = 1.0; ifac > 0; ifac--) {
      invw *= beta_r;
    }
  } else { /* unable to avoid exponential */
    invw = exp(w->ens_exp * log(beta));
  }

  return invw;
}


double at_distr_weights__calc_inv_weight(const at_distr_weights_t *w, double beta,
    double *neg_dlnwf_dbeta, double *pf, double *neg_df_dbeta)
{
  double f, invw, invwf, neg_df_dbeta_ = 0.0;

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

  zcom_util__exit_if (invwf > 1e6 || invwf < 1e-6, "bad invwf=%g, f=%g, beta=%g\n", invwf, beta);

  if (neg_dlnwf_dbeta != NULL) {
    *neg_dlnwf_dbeta = (w->ens_exp / beta)
                     + (*neg_df_dbeta / f);
  }

  return invwf;
}


#endif
