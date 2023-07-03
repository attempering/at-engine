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

#ifndef AT_MB_WIN_BASIC__SRC_H__
#define AT_MB_WIN_BASIC__SRC_H__


#include "at_mb_win_basic.h"
#include "../../distr/at_distr.h"



int at_mb_win__conf_init(at_mb_win_t* win, at_utils_conf_t *conf, at_mb_t *mb)
{
  at_distr_domain_t *domain = mb->distr->domain;
  int i, n = domain->n;

  at_utils_log__exit_if (n <= 0,
      conf->log,
      "failed validation: n %d > 0\n", n);

  win->n = n;

  /* bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  at_utils_conf__get_int(conf,
      "mbest-mbin-mode,mb-mbin-mode",
      &win->bwmod, 1,
      "bwmod");

  at_utils_log__exit_if ( !(win->bwmod >= 0 && win->bwmod <= 2),
      conf->log,
      "win->bwmod = %d: failed validation: win->bwmod >= 0 && win->bwmod <= 2\n",
      win->bwmod);

  if (win->bwmod == 1) {
    /* bwdel: delta lnT */
    at_utils_conf__get_double(conf,
        "mbest-delta-lnT,mb-del-lnT",
        &win->bwdel, 0.05,
        "bwdel");

    at_utils_log__exit_if ( !(win->bwdel > domain->beta_del/domain->beta_min),
        conf->log,
        "win->bwdel: failed validation: win->bwdel %g > %g, (beta-del %g)/(beta-min %g)\n",
        win->bwdel, domain->beta_del/domain->beta_min, domain->beta_del, domain->beta_min);
  }

  if (win->bwmod == 0) {
    /* bwdel: delta beta */
    at_utils_conf__get_double(conf,
        "mbest_delta_beta,mb-del-beta",
        &win->bwdel, 0.02,
        "bwdel");

    at_utils_log__exit_if ( !(win->bwdel > domain->beta_del),
        conf->log,
        "win->bwdel: failed validation: win->bwdel %g > beta-del %g\n",
        win->bwdel, domain->beta_del);
  }


  if (win->bwmod == 2) {
    /* bwdel: delta kT */
    at_utils_conf__get_double(conf,
        "mbest-delta-kT,mb-del-kT",
        &win->bwdel, 0.1,
        "bwdel");

    at_utils_log__exit_if ( !(win->bwdel > domain->beta_del/pow(domain->beta_min, 2.0)),
        conf->log,
        "win->bwdel: failed validation: win->bwdel %g > (beta-del %g)/(beta-min %g)^2\n",
        win->bwdel, domain->beta_del, domain->beta_min);
  }


  at_utils__new_arr(win->js_grid_unres, n+1, int);
  for (i = 0; i <= n; i++) {
    win->js_grid_unres[i] = 0;
  }

  at_utils__new_arr(win->jt_grid_unres, n+1, int);
  for (i = 0; i <= n; i++) {
    win->jt_grid_unres[i] = 0;
  }

  at_mb_win__make_unres_windows_for_grid_estimators(n, domain->barr, domain->beta_del,
      win->bwmod, win->bwdel,
      win->js_grid_unres, win->jt_grid_unres);

  at_utils__new_arr(win->js_bin, n, int);
  for (i = 0; i < n; i++) {
    win->js_bin[i] = 0;
  }

  at_utils__new_arr(win->jt_bin, n, int);
  for (i = 0; i < n; i++) {
    win->jt_bin[i] = 0;
  }

  at_mb_win__make_windows_for_bin_estimators(n, mb->use_sym_wins,
      win->js_grid_unres, win->jt_grid_unres,
      win->js_bin, win->jt_bin);

  /* initialize the mapping from bin index to the affected window indices */
  at_mb_win__init_bin2wins(win);

  win->max_win_bins = 0;
  for (i = 0; i < n; i++) {
    int win_bins = win->jt_bin[i] - win->js_bin[i];

    if (win_bins > win->max_win_bins) {
      win->max_win_bins = win_bins;
    }
  }

  at_utils__new_arr(win->js_grid_res, n+1, int);
  for (i = 0; i <= n; i++) {
    win->js_grid_res[i] = 0;
  }

  at_utils__new_arr(win->jt_grid_res, n+1, int);
  for (i = 0; i <= n; i++) {
    win->jt_grid_res[i] = 0;
  }

  at_mb_win__make_res_windows_for_grid_estimators(n,
      win->js_bin, win->jt_bin,
      win->js_grid_res, win->jt_grid_res);

  return 0;
}



static void at_mb_win_ids__clear(at_mb_win_ids_t *wi)
{
  wi->count = 0;
  wi->curr_id_ = 0;
  free(wi->ids);
  wi->ids = NULL;
}



void at_mb_win__finish(at_mb_win_t *win)
{
  int i, n = win->n;

  free(win->js_grid_unres);
  free(win->jt_grid_unres);

  free(win->js_bin);
  free(win->jt_bin);

  free(win->js_grid_res);
  free(win->jt_grid_res);

  for (i = 0; i < n; i++) {
    at_mb_win_ids__clear(win->bin2wins + i);
  }

  free(win->bin2wins);

  memset(win, 0, sizeof(*win));
}



void at_mb_win__manifest(const at_mb_win_t *win, at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.mb.win");

  /* 0: d(beta) 1: dT/T  2: d(kT) */
  at_utils_manifest__print_int(manifest, win->bwmod, "bwmod", "mb-mbin-mode");

  if (win->bwmod == 0) {
    at_utils_manifest__print_double(manifest, win->bwdel, "bwdel", "mb-del-beta");
  } else if (win->bwmod == 1) {
    at_utils_manifest__print_double(manifest, win->bwdel, "bwdel", "mb-del-lnT");
  } else if (win->bwmod == 2) {
    at_utils_manifest__print_double(manifest, win->bwdel, "bwdel", "mb-del-kT");
  }

  at_utils_manifest__print_int_arr(manifest, win->js_grid_unres, win->n+1, "js_grid_unres");

  at_utils_manifest__print_int_arr(manifest, win->jt_grid_unres, win->n+1, "jt_grid_unres");

  at_utils_manifest__print_int_arr(manifest, win->js_bin, win->n, "js_bin");

  at_utils_manifest__print_int_arr(manifest, win->jt_bin, win->n, "jt_bin");

  at_utils_manifest__print_int_arr(manifest, win->js_grid_res, win->n+1, "js_grid_res");

  at_utils_manifest__print_int_arr(manifest, win->jt_grid_res, win->n+1, "jt_grid_res");

  at_utils_manifest__pop_mod(manifest);

}



#endif
