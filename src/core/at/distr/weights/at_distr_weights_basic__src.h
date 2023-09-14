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

#ifndef AT_DISTR_WEIGHTS_BASIC__SRC_H__
#define AT_DISTR_WEIGHTS_BASIC__SRC_H__


#include "at_distr_weights.h"
#include "../domain/at_distr_domain.h"

#include "composite/at_distr_weights_composite__src.h"
#include "at_distr_weights_order__src.h"
#include "../../utils/at_utils.h"



int at_distr_weights__conf_init(
    at_distr_weights_t *w,
    at_distr_domain_t *domain,
    at_utils_conf_t *conf)
{
  w->beta_min = domain->beta_min;
  w->beta_max = domain->beta_max;
  w->n = domain->n;

  /* initialize a module-specific logger */
  at_utils_log__init_delegate(w->log, conf->log, "at.distr.weights");

  at_utils_conf__push_mod(conf, "at.distr.weights");

  //at_utils_log__info(conf->log, "w->n %d\n", w->n); getchar();

  at_utils_conf__get_double(conf,
      "ensemble-f-min",
      &w->f_min, 1e-6, "f_min");

  at_utils_conf__get_double(conf,
      "ensemble-f-max",
      &w->f_max, 1e+6, "f_max");

  at_utils_conf__get_double(conf,
      "ensemble-invwf-min",
      &w->invwf_min, 0.0, "invwf_min");

  at_utils_conf__get_double(conf,
      "ensemble-invwf-max",
      &w->invwf_max, 1e+3, "invwf_max");

  //printf("w %p\n", w); getchar();
  at_utils_conf__get_double(conf,
      "ensemble-factor,ensemble-exp,ensemble-exponent",
      &w->ens_exp, 1.0, "exponent");

  //fprintf(stderr, "ens_exp %g\n", w->ens_exp); getchar();

  at_distr_weights__init_ens_exp(w);

  at_utils_conf__get_int(conf,
      "ensemble-mode",
      &w->mode, 0, "mode");

  /* default values */
  w->beta0 = 0.5 * (w->beta_min + w->beta_max);
  w->inv_sigma2 = 1.0;
  w->c = 0.0;

  if (w->mode == AT_DISTR_WEIGHTS_MODE__GAUSSIAN) {

    at_utils_log__info(conf->log, "single-Gaussian distribution mode\n");

    at_utils_conf__get_double(conf,
        "ensemble-beta0",
        &w->beta0, 0.0, "ensemble_beta0");

    at_utils_conf__get_double(conf,
        "ensemble-sigma",
        &w->sigma, 0.0, "ensemble_sigma");

    if (w->sigma <= 0) {
      at_utils_log__error(conf->log, "sigma %lf is not positive!\n", w->sigma);
      goto ERR;
    }

    w->inv_sigma2 = 1.0/w->sigma/w->sigma;

  } else if (w->mode == AT_DISTR_WEIGHTS_MODE__EXPONENTIAL) {

    at_utils_log__info(w->log, "single-exponential distribution mode\n");

    at_utils_conf__get_double(conf,
        "ensemble-c",
        &w->c, 0.0, "ensemble_c");

  } else if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPOSITE) {

    at_distr_weights_composite__conf_init(w->composite, domain, conf);

  } else if(w->mode != 0) {

    at_utils_log__error(w->log, "invalid ensemble mode\n");
    goto ERR;

  }

  at_distr_weights__init_ens_w(w, domain);

  at_distr_weights__calc_q_order(w);

  at_utils_conf__pop_mod(conf);

  return 0;

ERR:
  return 1;
}


void at_distr_weights__manifest(const at_distr_weights_t *w, at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.distr.weights");

  at_utils_manifest__print_double(manifest, w->ens_exp, "ens_exp", "ensemble-factor");

  at_utils_manifest__print_double(manifest, w->f_min, "f_min", "ensemble-f-min");

  at_utils_manifest__print_double(manifest, w->f_max, "f_max", "ensemble-f-max");

  at_utils_manifest__print_double(manifest, w->invwf_min, "invwf_min", "ensemble-invwf-min");

  at_utils_manifest__print_double(manifest, w->invwf_max, "invwf_max", "ensemble-invwf-max");

  at_utils_manifest__print_int(manifest, w->mode, "mode", "ensemble-mode");

  if (w->mode == AT_DISTR_WEIGHTS_MODE__GAUSSIAN) {

    at_utils_manifest__print_double(manifest, w->beta0, "beta0", "ensemble-beta0");
    at_utils_manifest__print_double(manifest, w->sigma, "sigma", "ensemble-sigma");

  } else if (w->mode == AT_DISTR_WEIGHTS_MODE__EXPONENTIAL) {

    at_utils_manifest__print_double(manifest, w->c, "c", "ensemble-c");

  } else if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPOSITE) {

    at_distr_weights_composite__manifest(w->composite, manifest);

  }

  /* ens_w: array of ensemble weights at bin boundaries */
  at_utils_manifest__print_double_arr(manifest, w->ens_w, w->n, "ens_w");

  at_utils_manifest__print_double(manifest, w->q_order, "q_order", NULL);

  at_utils_manifest__pop_mod(manifest);
}



void at_distr_weights__finish(at_distr_weights_t *w)
{
  if (w->ens_w != NULL) {
    free(w->ens_w);
  }

  if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPOSITE) {
    at_distr_weights_composite__finish(w->composite);
  }

  at_utils_log__finish(w->log);
}



int at_distr_weights__get_n_components(
    const at_distr_weights_t *w)
{
  if (w->mode == AT_DISTR_WEIGHTS_MODE__COMPOSITE) {
    return w->composite->n_components;
  } else {
    return 0;
  }
}


#endif
