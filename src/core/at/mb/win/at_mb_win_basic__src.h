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

#include "../../../zcom/zcom.h"




int at_mb_win__cfg_init(at_mb_win_t* win, zcom_cfg_t *cfg, at_mb_t *mb)
{
  at_distr_domain_t *domain = mb->distr->domain;
  int i, n = domain->n;

  win->n = n;

  /* bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  win->bwmod = 1;

  if (cfg != NULL && 0 != zcom_cfg__get(cfg, &win->bwmod, "mbest_mbin_mode", "%d")) {
    fprintf(stderr, "assuming default: win->bwmod = 1, key: mbest_mbin_mode\n");
  }

  zcom_util__exit_if ( !(win->bwmod >= 0 && win->bwmod <= 2),
      "win->bwmod: failed validation: win->bwmod >= 0 && win->bwmod <= 2\n");


  if (win->bwmod == 1) {
    /* bwdel: delta lnT */
    win->bwdel = 0.05;

    zcom_util__exit_if (cfg != NULL && 0 != zcom_cfg__get(cfg, &win->bwdel, "mbest_delta_lnT", "%lf"),
        "missing var: win->bwdel, key: mbest_delta_lnT, fmt: %%lf\n");

    zcom_util__exit_if ( !(win->bwdel > domain->bdel/pow(domain->bmin, 1.0)),
        "win->bwdel: failed validation: win->bwdel %g > %g, domain->bdel %g /pow(domain->bmin %g , 1.0)\n",
        win->bwdel, domain->bdel/pow(domain->bmin, 1.0), domain->bdel, domain->bmin);
  }

  if (win->bwmod == 0) {
    /* bwdel: delta beta */
    win->bwdel = 0.02;

    zcom_util__exit_if (cfg != NULL && 0 != zcom_cfg__get(cfg, &win->bwdel, "mbest_delta_beta", "%lf"),
        "missing var: win->bwdel, key: mbest_delta_beta, fmt: %%lf\n");

    zcom_util__exit_if ( !(win->bwdel > domain->bdel/pow(domain->bmin, 0.0)),
        "win->bwdel: failed validation: win->bwdel > domain->bdel/pow(domain->bmin, 0.0)\n");
  }


  if (win->bwmod == 2) {
    /* bwdel: delta kT */
    win->bwdel = 0.1;

    zcom_util__exit_if (cfg != NULL && 0 != zcom_cfg__get(cfg, &win->bwdel, "mbest_delta_kT", "%lf"),
        "missing var: win->bwdel, key: mbest_delta_kT, fmt: %%lf\n");

    zcom_util__exit_if ( !(win->bwdel > domain->bdel/pow(domain->bmin, 2.0)),
        "win->bwdel: failed validation: win->bwdel > domain->bdel/pow(domain->bmin, 2.0)\n");
  }


  zcom_util__exit_if ((win->js_grid_unres = (int *) calloc(n + 1, sizeof(int))) == NULL,
      "no memory! var: domain->js_grid_unres, type: int\n");

  for (i = 0; i <= n; i++) {
    win->js_grid_unres[i] = 0;
  }

  zcom_util__exit_if ((win->jt_grid_unres = (int *) calloc(n + 1, sizeof(int))) == NULL,
      "no memory! var: win->jt_grid_unres, type: int\n");

  for (i = 0; i <= n; i++) {
    win->jt_grid_unres[i] = 0;
  }

  at_mb_win__make_unres_windows_for_grid_estimators(n, domain->barr, domain->bdel,
      win->bwmod, win->bwdel,
      win->js_grid_unres, win->jt_grid_unres);


  // Note: without the conversion of `n` to unsigned,
  // GCC issues the following warning
  // warning: argument 1 range [18446744071562067968, 18446744073709551615]
  // exceeds maximum object size 9223372036854775807 [-Walloc-size-larger-than=]
  // for the next calloc line

  zcom_util__exit_if ((win->js_bin = (int *) calloc((unsigned) n, sizeof(int))) == NULL,
      "no memory! var: win->js_bin, type: int\n");

  for (i = 0; i < n; i++) {
    win->js_bin[i] = 0;
  }

  zcom_util__exit_if ((win->jt_bin = (int *) calloc((unsigned) n, sizeof(int))) == NULL,
    "no memory! var: win->jt_bin, type: int\n");

  for (i = 0; i < n; i++) {
    win->jt_bin[i] = 0;
  }

  at_mb_win__make_windows_for_bin_estimators(n, mb->flags & MB_SYMWIN,
      win->js_grid_unres, win->jt_grid_unres,
      win->js_bin, win->jt_bin);

  /* initialize the mapping from bin index to the affected window indices */
  at_mb_win__init_bin2wins(win);



  zcom_util__exit_if ((win->js_grid_res = (int *) calloc(n + 1, sizeof(int))) == NULL,
      "no memory! var: mb->js_grid_res, type: int\n");

  for (i = 0; i <= n; i++) {
    win->js_grid_res[i] = 0;
  }

  zcom_util__exit_if ((win->jt_grid_res = (int *) calloc(n + 1, sizeof(int))) == NULL,
      "no memory! var: win->jt_grid_res, type: int\n");

  for (i = 0; i <= n; i++) {
    win->jt_grid_res[i] = 0;
  }

  at_mb_win__make_res_windows_for_grid_estimators(n,
      win->js_bin, win->jt_bin,
      win->js_grid_res, win->jt_grid_res);


  return 0;
}



void at_mb_win_ids__clear(at_mb_win_ids_t *wi)
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
  FILE *fp = manifest->fp;

  /* bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  fprintf(fp, "win->bwmod: int, %4d\n", win->bwmod);

  /* bwdel: delta lnT */
  fprintf(fp, "win->bwdel: double, %g\n", win->bwdel);

  at_utils_manifest__print_int_arr(manifest, win->js_grid_unres, win->n+1, "win->js_grid_unres");

  at_utils_manifest__print_int_arr(manifest, win->jt_grid_unres, win->n+1, "win->jt_grid_unres");

  at_utils_manifest__print_int_arr(manifest, win->js_bin, win->n, "win->js_bin");

  at_utils_manifest__print_int_arr(manifest, win->jt_bin, win->n, "win->jt_bin");

  at_utils_manifest__print_int_arr(manifest, win->js_grid_res, win->n+1, "win->js_grid_res");

  at_utils_manifest__print_int_arr(manifest, win->jt_grid_res, win->n+1, "win->jt_grid_res");
}



#endif
