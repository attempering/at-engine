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

#ifndef AT_MB_SHK__SRC_H__
#define AT_MB_SHK__SRC_H__

/* shrinking rate of the adaptive averaging scheme */

#include "at_mb_shk.h"
#include "io/at_mb_shk_io__src.h"

#include "../at_mb_basic.h"
#include "../../distr/at_distr.h"


int at_mb_shk__conf_init(at_mb_shk_t *shk, at_mb_t *mb, at_utils_conf_t *conf)
{
  at_distr_domain_t *domain = mb->distr->domain;
  at_distr_weights_t *w = mb->distr->weights;
  int i;

  at_utils_conf__push_mod(conf, "at.mb.shk");

  shk->n = domain->n;

  /* shk_base: current generic shrink amplitude */
  shk->base = 0.0;

  /* shk_win_adjusted: adjust shrink according to temperature window width */
  at_utils_conf__get_bool(conf,
      "shrink-mbin-adjust,shrink-win-adjusted",
      &shk->win_adjusted, AT__TRUE,
      "win_adjusted");

  at_utils_conf__get_double(conf,
      "shrink0,shrink-init",
      &shk->init, 0.01,
      "init");
  if (shk->init >= 0.9 || shk->init < 0.0) {
    at_utils_logger__error(conf->logger, "shk->init: failed validation: shk->init < 0.9 && shk->init >= 0.0\n");
    goto ERR;
  }

  at_utils_conf__get_double(conf,
      "shrink-max",
      &shk->max, 0.01,
      "max");
  if (shk->max >= 0.9 || shk->max < 0.0) {
    at_utils_logger__error(conf->logger, "shk->max: failed validation: shk->max < 0.9 && shk->max >= 0.0\n");
    goto ERR;
  }

  /* shk_win_mul: array used of modulation shrinking factors */
  at_utils__new_arr(shk->win_mul, shk->n, double);
  for (i = 0; i < shk->n; i++) {
    double beta_midpoint = at_distr_domain__get_bin_center(domain, i);
    double invwf = at_distr_weights__calc_inv_weight_bounded(
        w, beta_midpoint, NULL, NULL, NULL);
    int window_width = mb->win->jt_bin[i] - mb->win->js_bin[i];
    shk->win_mul[i] = invwf * mb->win->max_win_bins / window_width;
    //printf("shk ib %d, mul %g\n", i, shk->win_mul[i]);
  }
  //getchar();

  /* shk_mode: 0: const, 1: amp/t, 2: amp/t^exp */
  at_utils_conf__get_int(conf,
      "shrink-mode",
      &shk->mode, 1,
      "mode");
  if ( !(shk->mode >= 0 && shk->mode <= 2) ) {
    at_utils_logger__error(conf->logger, "shk->mode: failed validation: shk->mode >= 0 && shk->mode <= 2\n");
    goto ERR;
  }

  /* shk_min: minimal value for enforcing acc. sampling */
  at_utils_conf__get_double(conf,
      "shrinkmin,shrink-min",
      &shk->min, 0.0,
      "min");

  /* shk_stop: stop shrinking after this number of steps */
  at_utils_conf__get_double(conf,
      "shrinkstop,shrink-stop",
      &shk->stop, -1,
      "stop");

  /* shk_amp: amp t^(-exp) */
  at_utils_conf__get_double(conf,
      "shrinkamp,shrink-amp",
      &shk->amp, 0.1,
      "amp");

  /* shk_exp: amp t^(-exp) */
  at_utils_conf__get_double(conf,
      "shrinkexp,shrink-exp",
      &shk->exp, 1.0,
      "exp");

  at_utils_conf__pop_mod(conf);

  return 0;

ERR:

  return 1;
}



void at_mb_shk__finish(at_mb_shk_t *shk) {
  if (shk->win_mul != NULL) {
    free(shk->win_mul);
  }
}



void at_mb_shk__manifest(const at_mb_shk_t *shk, at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.mb.shk");

  /* current generic shrink amplitude */
  at_utils_manifest__print_double(manifest, shk->base, "base", NULL);

  /* adjust shrink according to temperature window width */
  at_utils_manifest__print_bool(manifest, shk->win_adjusted, "win_adjusted", "shrink-win-adjusted");

  /* array used of modulation shrinking factors */
  at_utils_manifest__print_double_arr(manifest, shk->win_mul, shk->n, "win_mul");

  /* shk_mode: 0: const, 1: amp/t, 2: amp/t^exp */
  at_utils_manifest__print_int(manifest, shk->mode, "mode", "shrink-mode");

  at_utils_manifest__print_double(manifest, shk->init, "init", "shrink-init");

  at_utils_manifest__print_double(manifest, shk->max, "max", "shrink-max");

  /* shk_min: minimal value for enforcing acc. sampling */
  at_utils_manifest__print_double(manifest, shk->min, "min", "shrink-min");

  /* shk_stop: stop shrinking after this number of steps */
  at_utils_manifest__print_double(manifest, shk->stop, "stop", "shrink-stop");

  if (shk->mode >= 1) {
    /* shk_amp: amp t^(-exp) */
    at_utils_manifest__print_double(manifest, shk->amp, "amp", "shrink-amp");
  }
  if (shk->mode >= 2) {
    /* shk_exp: amp t^(-exp) */
    at_utils_manifest__print_double(manifest, shk->exp, "exp", "shrink-exp");
  }

  at_utils_manifest__pop_mod(manifest);
}


/* compute the temperature-independent shrinking factor */
double at_mb_shk__calc_shk_base(at_mb_shk_t *shk, double total_visits)
{
  double x, shk_val;

  if (shk->stop >= 0
   && total_visits > shk->stop)
  {
    return 0.0;
  }
  else if (shk->mode == 0)
  {
    /* shk_min is usually 0, which disables adaptive averaging */
    return shk->min;
  }

  shk_val = shk->max;

  if (total_visits > 10 * shk->n) {

    x = total_visits / shk->n;

    if (shk->mode == 1)
    {
      /* 1/t */
      x = shk->amp / x;
    }
    else if (shk->mode == 2)
    {
      /* amp / t^shk_exp */
      x = shk->amp / pow(x, shk->exp);
    }
    else
    {
      zcom_utils__exit_if(1, "invalid shk_mode: %d\n", shk->mode);
    }

    if (x < shk_val) {
      shk_val = x;
    }

    if (shk_val < shk->min) {
      shk_val = shk->min;
    }

  }

  return shk->base = shk_val;
}


/* compute the window-adjusted shrinking factor */
double at_mb_shk__calc_inv_gamma(at_mb_shk_t *shk, double total_visits, int ib)
{
  double shk_val;

  shk_val = at_mb_shk__calc_shk_base(shk, total_visits); /* compute the unadjusted */

  //fprintf(stderr, "shk_val %d %g\n", ib, shk_val);

  if (shk->win_adjusted) { /* multiply the gauge */
    zcom_utils__exit_if (shk->win_mul == NULL, "window multiplier is null\n");
    zcom_utils__exit_if (ib < 0 || ib >= shk->n, "index %d out of range\n", ib);

    shk_val *= shk->win_mul[ib];

    //fprintf(stderr, "shk_mul %d %g\n", ib, shk->win_mul[ib]); getchar();

    if (shk_val > shk->max) {
      shk_val = shk->max;
    }
  }

  return 1.0 / (1.0 - shk_val);
}



#endif
