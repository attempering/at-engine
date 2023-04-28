/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

#include "../at_mb_basic.h"

#include <stdlib.h>

#include "../../../zcom/cfg/cfg.h"


int at_mb_shk__cfg_init(at_mb_shk_t *shk, zcom_cfg_t *cfg, at_mb_t *mb, int m, int silent)
{
  int i;

  shk->n = mb->n;

  /* shk_base: current generic shrink amplitude */
  shk->base = 0.0;
  /* shk_window_adjusted: adjust shrink according to temperature window width */
  shk->window_adjusted = 1;
  if (0 != zcom_cfg__get(cfg, &shk->window_adjusted, "shrink_mbin_adjust", "%d")) {
    if (!silent) fprintf(stderr, "assuming default: mb->shk->window_adjusted = 1, key: shrink_mbin_adjust\n");
  }

  /* shk_max: initial and maximal shrink (adjusted) */
  shk->max = 0.01;
  if (0 != zcom_cfg__get(cfg, &shk->max, "shrink0", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: mb->shk->max = 0.01, key: shrink0\n");
  }
  if ( !(shk->max < 0.9 && shk->max >= 0.0) ) {
    fprintf(stderr, "shk->max: failed validation: mb->shk->max < 0.9 && shk->max >= 0.0\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* shk_win_multiplier: array used of modulation shrinking factors */
  if ((shk->win_multiplier = (double *) calloc((shk->n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "no memory! var: shk->win_multiplier, type: double\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < shk->n; i++) {
    double beta_midpoint = 0.5*(mb->barr[i] + mb->barr[i+1]);
    double invwf = at_mb_betadist__calc_inv_weight(mb->betadist, beta_midpoint, NULL, NULL, NULL);
    int window_width = mb->win->jt_bin[i] - mb->win->js_bin[i];
    shk->win_multiplier[i] = invwf * mb->accum->m / window_width;
  }

  /* shk_mode: 0: const, 1: amp/t, 2: amp/t^exp */
  shk->mode = 1;
  if (0 != zcom_cfg__get(cfg, &shk->mode, "shrink_mode", "%d")) {
    if (!silent) fprintf(stderr, "assuming default: mb->shk->mode = 1, key: shrink_mode\n");
  }
  if ( !(shk->mode >= 0 && shk->mode <= 2) ) {
    fprintf(stderr, "shk->mode: failed validation: mb->shk->mode >= 0 && shk->mode <= 2\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* shk_min: minimal value for enforcing acc. sampling */
  shk->min = 0.0;
  if (0 != zcom_cfg__get(cfg, &shk->min, "shrinkmin", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: mb->shk->min = 0.0, key: shrinkmin\n");
  }

  /* shk_stop: stop shrinking after this number of steps */
  shk->stop = -1;
  if (0 != zcom_cfg__get(cfg, &shk->stop, "shrinkstop", "%d")) {
    if (!silent) fprintf(stderr, "assuming default: mb->shk->stop = -1, key: shrinkstop\n");
  }

  /* shk_amp: amp t^(-exp) */
  shk->amp = 0.1;
  if (shk->mode >= 1) {
    if (0 != zcom_cfg__get(cfg, &shk->amp, "shrinkamp", "%lf")) {
      if (!silent) fprintf(stderr, "assuming default: mb->shk->amp = 0.1, key: shrinkamp\n");
    }
  }

  /* shk_exp: amp t^(-exp) */
  shk->exp = 1.0;
  if (shk->mode >= 2) {
    if (0 != zcom_cfg__get(cfg, &shk->exp, "shrinkexp", "%lf")) {
      if (!silent) fprintf(stderr, "assuming default: mb->shk->exp = 1.0, key: shrinkexp\n");
    }
  }

  return 0;

ERR:

  return 1;
}



void at_mb_shk__finish(at_mb_shk_t *shk) {
  if (shk->win_multiplier != NULL) {
    free(shk->win_multiplier);
  }
}



void at_mb_shk__manifest(const at_mb_shk_t *shk, at_utils_manifest_t *manifest)
{
  FILE *fp = manifest->fp;

  /* shk_base: current generic shrink amplitude */
  fprintf(fp, "shk->base: double, %g\n", shk->base);

  /* shk_window_adjusted: adjust shrink according to temperature window width */
  fprintf(fp, "shk->window_adjusted: int, %4d\n", shk->window_adjusted);

  /* shk_max: initial and maximal shrink (adjusted) */
  fprintf(fp, "shk->max: double, %g\n", shk->max);

  /* shk_win_multiplier: array used of modulation shrinking factors */
  at_utils_manifest__print_double_arr(manifest, shk->win_multiplier, shk->n, "shk->win_multiplier");

  /* shk_mode: 0: const, 1: amp/t, 2: amp/t^exp */
  fprintf(fp, "shk->mode: int, %4d\n", shk->mode);

  /* shk_min: minimal value for enforcing acc. sampling */
  fprintf(fp, "shk->min: double, %g\n", shk->min);

  /* shk_stop: stop shrinking after this number of steps */
  fprintf(fp, "shk->stop: int, %4d\n", shk->stop);

  if (shk->mode >= 1) {
    /* shk_amp: amp t^(-exp) */
    fprintf(fp, "shk->amp: double, %g\n", shk->amp);
  }
  if (shk->mode >= 2) {
    /* shk_exp: amp t^(-exp) */
    fprintf(fp, "shk->exp: double, %g\n", shk->exp);
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
double at_mb_shk__calc_inverse_gamma(at_mb_shk_t *shk, double total_visits, int ib)
{
  double shk_val;

  shk_val = at_mb_shk__calc_shk_base(shk, total_visits); /* compute the unadjusted */

  if (shk->window_adjusted) { /* multiply the gauge */
    zcom_util__exit_if (shk->win_multiplier == NULL, "window multiplier is null\n");
    zcom_util__exit_if (ib < 0 || ib >= shk->n, "index %d out of range\n", ib);

    shk_val *= shk->win_multiplier[ib];

    if (shk_val > shk->max) {
      shk_val = shk->max;
    }
  }

  return 1.0 / (1.0 - shk_val);
}



#endif
