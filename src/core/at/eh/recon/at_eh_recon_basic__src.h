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

#ifndef AT_EH_RECON_BASIC__SRC_H__
#define AT_EH_RECON_BASIC__SRC_H__

#include "at_eh_recon_basic.h"
#include "../../mb/win/at_mb_win.h"
#include "../../distr/at_distr.h"

int at_eh_recon__conf_init(at_eh_recon_t *recon, at_eh_t *eh,
    at_utils_conf_t *conf)
{
  at_distr_t *distr = eh->mb->distr;
  at_distr_domain_t *domain = distr->domain;
  int i;

  recon->n = eh->n;

  recon->e_min = eh->e_min;
  recon->e_max = eh->e_max;
  recon->e_del = eh->e_del;
  recon->e_n = eh->n;

  at_utils_conf__push_mod(conf, "at.eh.recon");

  /* interval of reconstructing energy histograms */
  at_utils_conf__get_int(conf,
      "ehist-skip,ehist-recon-stride",
      &recon->stride, 10,
      "stride");

  /* eh_bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  at_utils_conf__get_int(conf,
      "ehist_mbin_mode,ehist-recon-mode",
      &recon->bwmod, 1,
      "bwmod");

  if ( !(recon->bwmod >= 0 && recon->bwmod <= 2) ) {
    at_utils_log__error(conf->log, "recon->bwmod: failed validation: recon->bwmod >= 0 && recon->bwmod <= 2\n");
    goto ERR;
  }

  /* eh_bwdel: delta lnT */
  if (recon->bwmod == 1) {
    at_utils_conf__get_double(conf,
        "ehist_delta_lnT,ehist-del-lnT",
        &recon->bwdel, 0.05,
        "bwdel");

    if ( !(recon->bwdel > domain->beta_del/domain->beta_min) ) {
      at_utils_log__error(conf->log, "recon->bwdel: failed validation: recon->bwdel %g > (beta-del %g)/(beta-min %g)\n",
          recon->bwdel, domain->beta_del, domain->beta_min);
      goto ERR;
    }
  }

  /* eh_bwdel: delta beta */
  if (recon->bwmod == 0) {
    at_utils_conf__get_double(conf,
        "ehist_delta_beta,ehist-del-beta",
        &recon->bwdel, 0.02,
        "bwdel");

    if ( !(recon->bwdel > distr->domain->beta_del) ) {
      at_utils_log__error(conf->log, "recon->bwdel: failed validation: (recon->bwdel %g) > (beta-del %g)\n",
          recon->bwdel, domain->beta_del);
      goto ERR;
    }
  }

  /* eh_bwdel: delta kT */
  if (recon->bwmod == 2) {
    at_utils_conf__get_double(conf,
        "ehist_delta_kT,ehist-del-kT",
        &recon->bwdel, 0.10,
        "bwdel");

    if ( !(recon->bwdel > distr->domain->beta_del/pow(distr->domain->beta_min, 2.0)) ) {
      at_utils_log__error(conf->log, "recon->bwdel: failed validation: (recon->bwdel %g) > (beta-del %g)/(beta-min %g)^2\n",
          recon->bwdel, domain->beta_del, domain->beta_min);
      goto ERR;
    }
  }


  /* name of reconstructed energy histogram */
  at_utils_conf__get_filename(conf,
      "ehist-recon-file,ehist_mbin_file",
      &recon->file, "hist-recon.dat",
      "file");

  /* eh_recon: temporary space for reconstructing histogram */
  {
    at_utils__new_arr(recon->recon, recon->e_n, double);
    for (i = 0; i < recon->e_n; i++) {
      recon->recon[i] = 0.0;
    }
  }

  /* indices for grid estimators */
  {
    at_utils__new_arr(recon->is, recon->e_n + 1, int);
    for (i = 0; i < recon->n + 1; i++) {
      recon->is[i] = 0;
    }

    at_utils__new_arr(recon->it, recon->e_n + 1, int);
    for (i = 0; i < recon->n + 1; i++) {
      recon->it[i] = 0;
    }

    //fprintf(stderr, "recon->is %p, recon->it %p, %s:%d\n", recon->is, recon->it, __FILE__, __LINE__);

    // windows for the reconstructed energy histograms
    at_mb_win__make_unres_windows_for_grid_estimators(
        distr->domain->n, distr->domain->barr, distr->domain->beta_del,
        recon->bwmod, recon->bwdel, recon->is, recon->it);
  }

  at_utils_conf__pop_mod(conf);

  return 0;

ERR:
  return -1;
}


void at_eh_recon__clear(at_eh_recon_t *recon)
{
  int i;

  for (i = 0; i < recon->e_n; i++) {
    recon->recon[i] = 0.0;
  }

}


void at_eh_recon__finish(at_eh_recon_t *recon)
{
  /* when recon->mode == 0, data members are not allocated */
  if (recon->recon  != NULL) free(recon->recon);
  if (recon->is     != NULL) free(recon->is);
  if (recon->it     != NULL) free(recon->it);
}


void at_eh_recon__manifest(const at_eh_recon_t* recon, at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.eh.recon");

  /* interval of reconstructing energy histograms */
  at_utils_manifest__print_int(manifest, recon->stride, "stride", "ehist-recon-stride");

  /* eh_bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  at_utils_manifest__print_double(manifest, recon->bwmod, "bwmod", "ehist-recon-mode");

  if (recon->bwmod == 0) {
    at_utils_manifest__print_double(manifest, recon->bwdel, "bwdel", "ehist-del-beta");
  } else if (recon->bwmod == 1) {
    at_utils_manifest__print_double(manifest, recon->bwdel, "bwdel", "ehist-del-lnT");
  } else if (recon->bwmod == 2) {
    at_utils_manifest__print_double(manifest, recon->bwdel, "bwdel", "ehist-del-kT");
  }

  /* name of reconstructed energy histogram */
  at_utils_manifest__print_str(manifest, recon->file, "file", "ehist-recon-file");

  /* eh_recon: temporary space for reconstructing histogram */
  at_utils_manifest__print_double_arr(manifest, recon->recon, recon->e_n, "recon");

  /* eh_is: indices for temperature windows (lower) */
  at_utils_manifest__print_int_arr(manifest, recon->is, recon->n + 1, "is");

  /* eh_it: indices for temperature windows (higher) */
  at_utils_manifest__print_int_arr(manifest, recon->it, recon->n + 1, "it");

  at_utils_manifest__pop_mod(manifest);
}



#endif
