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

#include <stdlib.h>

#include "../../../zcom/zcom.h"


int at_mb_shk__cfg_init(at_mb_shk_t *shk, zcom_cfg_t *cfg, at_mb_t *mb, at_bool_t verbose)
{
  at_distr_domain_t *domain = mb->distr->domain;
  at_distr_weights_t *w = mb->distr->weights;
  int i;

  shk->n = domain->n;

  /* shk_base: current generic shrink amplitude */
  shk->base = 0.0;
  /* shk_win_adjusted: adjust shrink according to temperature window width */
  shk->win_adjusted = AT__TRUE;
  if (0 != zcom_cfg__get(cfg, &shk->win_adjusted, "shrink-mbin-adjust", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.mb.shk: assuming default mb->shk->win_adjusted = 1, key: shrink-mbin-adjust\n");
  }

  shk->init = 0.01;
  if (0 != zcom_cfg__get(cfg, &shk->init, "shrink0,shrink-init", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at.mb.shk: assuming default mb->shk->init = 0.01, key: shrink-init\n");
  }
  if ( !(shk->init < 0.9 && shk->init >= 0.0) ) {
    fprintf(stderr, "Error@at.mb.shk: shk->max: failed validation: mb->shk->init < 0.9 && shk->init >= 0.0\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  shk->max = 0.01;
  if (0 != zcom_cfg__get(cfg, &shk->max, "shrink-max", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at.mb.shk: assuming default mb->shk->max = 0.01, key: shrink-init\n");
  }
  if ( !(shk->max < 0.9 && shk->max >= 0.0) ) {
    fprintf(stderr, "Error@at.mb.shk: shk->max: failed validation: mb->shk->max < 0.9 && shk->max >= 0.0\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* shk_win_mul: array used of modulation shrinking factors */
  if ((shk->win_mul = (double *) calloc(shk->n, sizeof(double))) == NULL) {
    fprintf(stderr, "Error@at.mb.shk: no memory! var: shk->win_mul, type: double\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < shk->n; i++) {
    double beta_midpoint = at_distr_domain__get_bin_center(domain, i);
    double invwf = at_distr_weights__calc_inv_weight(w, beta_midpoint, NULL, NULL, NULL);
    int window_width = mb->win->jt_bin[i] - mb->win->js_bin[i];
    shk->win_mul[i] = invwf * mb->win->max_win_bins / window_width;
    //printf("shk ib %d, mul %g\n", i, shk->win_mul[i]);
  }
  //getchar();

  /* shk_mode: 0: const, 1: amp/t, 2: amp/t^exp */
  shk->mode = 1;
  if (0 != zcom_cfg__get(cfg, &shk->mode, "shrink-mode", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.mb.shk: assuming default mb->shk->mode = 1, key: shrink-mode\n");
  }
  if ( !(shk->mode >= 0 && shk->mode <= 2) ) {
    fprintf(stderr, "Error@at.mb.shk: shk->mode: failed validation: mb->shk->mode >= 0 && shk->mode <= 2\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* shk_min: minimal value for enforcing acc. sampling */
  shk->min = 0.0;
  if (0 != zcom_cfg__get(cfg, &shk->min, "shrinkmin,shrink-min", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at.mb.shk: assuming default mb->shk->min = 0.0, key: shrink-min\n");
  }

  /* shk_stop: stop shrinking after this number of steps */
  shk->stop = -1;
  if (0 != zcom_cfg__get(cfg, &shk->stop, "shrinkstop,shrink-stop", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.mb.shk: assuming default mb->shk->stop = -1, key: shrink-stop\n");
  }

  /* shk_amp: amp t^(-exp) */
  shk->amp = 0.1;
  if (shk->mode >= 1) {
    if (0 != zcom_cfg__get(cfg, &shk->amp, "shrinkamp,shrink-amp", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.mb.shk: assuming default mb->shk->amp = 0.1, key: shrink-amp\n");
    }
  }

  /* shk_exp: amp t^(-exp) */
  shk->exp = 1.0;
  if (shk->mode >= 2) {
    if (0 != zcom_cfg__get(cfg, &shk->exp, "shrinkexp,shrink-exp", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.mb.shk: assuming default mb->shk->exp = 1.0, key: shrink-exp\n");
    }
  }

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
  /* current generic shrink amplitude */
  at_utils_manifest__print_double(manifest, shk->base, "mb->shk->base", NULL);

  /* adjust shrink according to temperature window width */
  at_utils_manifest__print_bool(manifest, shk->win_adjusted, "mb->shk->win_adjusted", "shrink-win-adjusted");

  /* array used of modulation shrinking factors */
  at_utils_manifest__print_double_arr(manifest, shk->win_mul, shk->n, "mb->shk->win_mul");

  /* shk_mode: 0: const, 1: amp/t, 2: amp/t^exp */
  at_utils_manifest__print_int(manifest, shk->mode, "mb->shk->mode", "shrink-mode");

  at_utils_manifest__print_double(manifest, shk->init, "mb->shk->init", "shrink-init");

  at_utils_manifest__print_double(manifest, shk->max, "mb->shk->max", "shrink-max");

  /* shk_min: minimal value for enforcing acc. sampling */
  at_utils_manifest__print_double(manifest, shk->min, "mb->shk->min", "shrink-min");

  /* shk_stop: stop shrinking after this number of steps */
  at_utils_manifest__print_double(manifest, shk->stop, "mb->shk->stop", "shrink-stop");

  if (shk->mode >= 1) {
    /* shk_amp: amp t^(-exp) */
    at_utils_manifest__print_double(manifest, shk->amp, "mb->shk->amp", "shrink-amp");
  }
  if (shk->mode >= 2) {
    /* shk_exp: amp t^(-exp) */
    at_utils_manifest__print_double(manifest, shk->exp, "mb->shk->exp", "shrink-exp");
  }
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
      zcom_util__exit_if(1, "invalid shk_mode: %d\n", shk->mode);
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
    zcom_util__exit_if (shk->win_mul == NULL, "window multiplier is null\n");
    zcom_util__exit_if (ib < 0 || ib >= shk->n, "index %d out of range\n", ib);

    shk_val *= shk->win_mul[ib];

    //fprintf(stderr, "shk_mul %d %g\n", ib, shk->win_mul[ib]); getchar();

    if (shk_val > shk->max) {
      shk_val = shk->max;
    }
  }

  return 1.0 / (1.0 - shk_val);
}



#endif
