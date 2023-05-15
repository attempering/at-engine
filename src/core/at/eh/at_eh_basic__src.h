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

#ifndef AT_EH_BASIC__SRC_H__
#define AT_EH_BASIC__SRC_H__

#include "at_eh_basic.h"
#include "../mb/win/at_mb_win.h"
#include "../distr/at_distr.h"

#include "../../zcom/zcom.h"


int at_eh__cfg_init(at_eh_t *eh, at_mb_t *mb, zcom_cfg_t *cfg,
    zcom_ssm_t *ssm, const char *data_dir, at_bool_t verbose)
{
  int i;
  at_distr_t *distr = mb->distr;

  eh->mb = mb;
  eh->n = distr->domain->n;

  eh->min_real = 0;
  eh->max_real = 0;

  /* eh_mode: 0: disable; 1: simple histogram */
  eh->mode = 0;
  if (0 != zcom_cfg__get(cfg, &eh->mode, "ehist_mode", "%d")) {
    if (verbose) fprintf(stderr, "at-info: assuming default eh->mode = 0, key: ehist_mode\n");
  }
  if ( !(eh->mode == 0 || eh->mode == 1) ) {
    fprintf(stderr, "eh->mode: failed validation: eh->mode == 0 || eh->mode == 1\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* eh_skip: interval of reconstructing energy histograms */
  eh->skip = 10;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->skip, "ehist_skip", "%d")) {
      fprintf(stderr, "at-info: assuming default eh->skip = 10, key: ehist_skip\n");
    }
    if ( !(eh->skip > 0) ) {
      fprintf(stderr, "eh->skip: failed validation: eh->skip > 0\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  eh->bwmod = 1;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->bwmod, "ehist_mbin_mode", "%d")) {
      fprintf(stderr, "at-info: assuming default eh->bwmod = 1, key: ehist_mbin_mode\n");
    }
    if ( !(eh->bwmod >= 0 && eh->bwmod <= 2) ) {
      fprintf(stderr, "eh->bwmod: failed validation: eh->bwmod >= 0 && eh->bwmod <= 2\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwdel: delta lnT */
  eh->bwdel = 0.05;
  if (eh->mode && eh->bwmod == 1) {
    if (0 != zcom_cfg__get(cfg, &eh->bwdel, "ehist_delta_lnT", "%lf")) {
      fprintf(stderr, "at-info: assuming default eh->bwdel = %g, key: ehist_delta_lnT, fmt: %%lf\n", eh->bwdel);
    }
    if ( !(eh->bwdel > distr->domain->beta_del/distr->domain->beta_min) ) {
      fprintf(stderr, "eh->bwdel: failed validation: eh->bwdel > beta_del/beta_min\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwdel: delta beta */
  if (eh->mode && eh->bwmod == 0) {
    eh->bwdel = 0.02;
    if (0 != zcom_cfg__get(cfg, &eh->bwdel, "ehist_delta_beta", "%lf")) {
      fprintf(stderr, "at-info: assuming default eh->bwdel = %g, key: ehist_delta_beta, fmt: %%lf\n", eh->bwdel);
    }
    if ( !(eh->bwdel > distr->domain->beta_del) ) {
      fprintf(stderr, "eh->bwdel: failed validation: eh->bwdel > beta_del\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwdel: delta kT */
  if (eh->mode && eh->bwmod == 2) {
    eh->bwdel = 0.10;
    if (0 != zcom_cfg__get(cfg, &eh->bwdel, "ehist_delta_kT", "%lf")) {
      fprintf(stderr, "at-info: assuming default eh->bwdel = %g, key: ehist_delta_kT, fmt: %%lf\n", eh->bwdel);
    }
    if ( !(eh->bwdel > distr->domain->beta_del/pow(distr->domain->beta_min, 2.0)) ) {
      fprintf(stderr, "eh->bwdel: failed validation: eh->bwdel > beta_del/beta_min^2\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_min: minimal energy */
  eh->min = -12.6e4;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->min, "ehist_min", "%lf")) {
      fprintf(stderr, "at-info: assuming default eh->min = -12.6e4, key: ehist_min\n");
    }
  }
  eh->min_real = eh->min;

  /* eh_max: maximal energy */
  eh->max = -9.0e4;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->max, "ehist_max", "%lf")) {
      fprintf(stderr, "at-info: assuming default eh->max = -9.0e4, key: ehist_max\n");
    }
    if ( !(eh->max > eh->min) ) {
      fprintf(stderr, "eh->max: failed validation: eh->max > eh->min\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }
  eh->max_real = eh->max;

  /* eh_del: energy bin size */
  eh->del = 20.0;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->del, "ehist_del", "%lf")) {
      fprintf(stderr, "at-info: assuming default eh->del = 20.0, key: ehist_del\n");
    }
    if ( !(eh->del > 0) ) {
      fprintf(stderr, "eh->del: failed validation: eh->del > 0\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_cnt: number of energy bins */
  eh->cnt = (int)((eh->max-eh->min)/eh->del - 1e-5 + 1);
  /* eh_binary: binary format for ehist file */
  eh->binary = 1;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->binary, "ehist_binary", "%d")) {
      fprintf(stderr, "at-info: assuming default eh->binary = 1, key: ehist_binary\n");
    }
  }

  /* eh_nst_save: interval of writing histogrm files */
  eh->nst_save = 100000;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->nst_save, "nsthist", "%d")) {
      fprintf(stderr, "at-info: assuming default eh->nst_save = 100000, key: nsthist\n");
    }
  }

  /* eh_file: name of ehist file */
  eh->file = NULL;
  if (eh->mode) {
    char *fn_eh = "hist.dat";
    if (0 != zcom_cfg__get(cfg, &fn_eh, "ehist_file", "%s")) {
      fprintf(stderr, "at-info: assuming default eh->file = \"%s\", key: ehist_file\n", fn_eh);
    }
    eh->file = at_utils__make_output_filename(ssm, data_dir, fn_eh);
  }

  /* eh_rfile: name of reconstructed energy histogram */
  eh->rfile = NULL;
  if (eh->mode) {
    char *fn_eh_mb = "hist_mb.dat";
    if (0 != zcom_cfg__get(cfg, &fn_eh_mb, "ehist_mbin_file", "%s")) {
      fprintf(stderr, "at-info: assuming default eh->rfile = \"%s\", key: ehist_mbin_file\n", fn_eh_mb);
    }
    eh->rfile = at_utils__make_output_filename(ssm, data_dir, fn_eh_mb);
  }

  /* eh_his: energy histogram data */
  eh->his = NULL;
  if (eh->mode) {
    if ((eh->his = (double *) calloc((eh->n*eh->cnt + 1), sizeof(double))) == NULL) {
      fprintf(stderr, "at->error: no memory! var: eh->his, type: double\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->n*eh->cnt; i++)
      eh->his[i] = 0.0;
  }
  /* eh_recon: temporary space for reconstructing histogram */
  eh->recon = NULL;
  if (eh->mode) {
    if ((eh->recon = (double *) calloc((eh->cnt + 1), sizeof(double))) == NULL) {
      fprintf(stderr, "at->error: no memory! var: eh->recon, type: double\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->cnt; i++)
      eh->recon[i] = 0.0;
  }
  /* eh_is: indices for temperature windows (lower) */
  eh->is = NULL;
  if (eh->mode) {
    if ((eh->is = (int *) calloc((eh->n + 2), sizeof(int))) == NULL) {
      fprintf(stderr, "at->error: no memory! var: eh->is, type: int\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->n + 1; i++) {
      eh->is[i] = 0;
    }
  }

  /* eh_it: indices for temperature windows (higher) */
  eh->it = NULL;
  if (eh->mode) {
    if ((eh->it = (int *) calloc((eh->n + 2), sizeof(int))) == NULL) {
      fprintf(stderr, "at->error: no memory! var: eh->it, type: int\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->n + 1; i++) {
      eh->it[i] = 0;
    }

    //fprintf(stderr, "eh->is %p, eh->it %p, %s:%d\n", eh->is, eh->it, __FILE__, __LINE__);

    // windows for the reconstructed energy histograms
    at_mb_win__make_unres_windows_for_grid_estimators(
        distr->domain->n, distr->domain->barr, distr->domain->beta_del,
        eh->bwmod, eh->bwdel, eh->is, eh->it);
  }

  if (eh->mode) {

    /* add a half energy bin width in output */
    eh->add_half_ebin = AT__TRUE;
    if (0 != zcom_cfg__get(cfg, &eh->add_half_ebin, "ehist_addahalf", "%u")) {
      fprintf(stderr, "at-info: assuming default eh->add_a_half = 1, key: ehist_addahalf\n");
    }

    /* keep zero margins */
    eh->keep_margins = AT__FALSE;
    if (0 != zcom_cfg__get(cfg, &eh->keep_margins, "ehist_keepedge", "%u")) {
      fprintf(stderr, "at-info: assuming default eh->keep_margins = 0, key: ehist_keepedge\n");
    }

    /* do not output zeroes */
    eh->no_zeros = 0;
    if (0 != zcom_cfg__get(cfg, &eh->no_zeros, "ehist_nozeroes", "%u")) {
      fprintf(stderr, "at-info: assuming default eh->no_zeros = 0, key: ehist_nozeroes\n");
    }
  }

  return 0;

ERR:
  return -1;
}


void at_eh__clear(at_eh_t *eh)
{
  int i;

  if (eh->mode <= 0) {
    return;
  }

  if (eh->mode) {
    for (i = 0; i < eh->n*eh->cnt; i++) {
      eh->his[i] = 0.0;
    }

    for (i = 0; i < eh->cnt; i++) {
      eh->recon[i] = 0.0;
    }
  }

}


void at_eh__finish(at_eh_t *eh)
{
  /* when eh->mode == 0, data members are not allocated */
  if (eh->mode != 0) {
    if (eh->his    != NULL) free(eh->his);
    if (eh->recon  != NULL) free(eh->recon);
    if (eh->is     != NULL) free(eh->is);
    if (eh->it     != NULL) free(eh->it);
  }
}


void at_eh__manifest(const at_eh_t* eh, at_utils_manifest_t *manifest)
{
  /* eh_mode: 0: disable; 1: simple histogram */
  at_utils_manifest__print_int(manifest, eh->mode, "eh->mode", "ehist_mode");

  if (eh->mode == 0) {
    return;
  }

  /* interval of reconstructing energy histograms */
  at_utils_manifest__print_int(manifest, eh->skip, "eh->skip", "ehist_skip");

  /* eh_bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  at_utils_manifest__print_double(manifest, eh->bwmod, "eh->bwmod", "ehist_mbin_mode");

  if (eh->bwmod == 0) {
    at_utils_manifest__print_double(manifest, eh->bwdel, "eh->bwdel", "ehist_delta_beta");
  } else if (eh->bwmod == 1) {
    at_utils_manifest__print_double(manifest, eh->bwdel, "eh->bwdel", "ehist_delta_lnT");
  } else if (eh->bwmod == 2) {
    at_utils_manifest__print_double(manifest, eh->bwdel, "eh->bwdel", "ehist_delta_kT");
  }

  /* eh_min: minimal energy */
  at_utils_manifest__print_double(manifest, eh->min, "eh->min", "ehist_min");

  /* eh_max: maximal energy */
  at_utils_manifest__print_double(manifest, eh->max, "eh->max", "ehist_max");

  /* eh_del: energy bin size */
  at_utils_manifest__print_double(manifest, eh->del, "eh->del", "ehist_del");

  /* eh_cnt: number of energy bins */
  at_utils_manifest__print_int(manifest, eh->cnt, "eh->cnt", NULL);

  /* eh_binary: binary format for ehist file */
  at_utils_manifest__print_bool(manifest, eh->binary, "eh->binary", "ehist_binary");

  /* eh_nst_save: interval of writing histogram files */
  at_utils_manifest__print_int(manifest, eh->nst_save, "eh->nst_save", "nsthist");

  /* name of ehist file */
  at_utils_manifest__print_str(manifest, eh->file, "eh->file", "ehist_file");

  /* name of reconstructed energy histogram */
  at_utils_manifest__print_str(manifest, eh->rfile, "eh->rfile", "ehist_mbin_file");

  /* eh_his: energy histogram data */
  at_utils_manifest__print_double_arr(manifest, eh->his, eh->n*eh->cnt, "eh->his");

  /* eh_recon: temporary space for reconstructing histogram */
  at_utils_manifest__print_double_arr(manifest, eh->recon, eh->cnt, "eh->recon");

  /* eh_is: indices for temperature windows (lower) */
  at_utils_manifest__print_int_arr(manifest, eh->is, eh->n + 1, "eh->is");

  /* eh_it: indices for temperature windows (higher) */
  at_utils_manifest__print_int_arr(manifest, eh->it, eh->n + 1, "eh->it");

  /* whether to add a half energy bin width in output */
  at_utils_manifest__print_bool(manifest, eh->add_half_ebin, "eh->add_half_ebin", "ehist_addahalf");

  /* whether to keep empty margin */
  at_utils_manifest__print_bool(manifest, eh->keep_margins, "eh->keep_margins", "ehist_keepedge");

  /* whether to include empty energy bins in the output */
  at_utils_manifest__print_bool(manifest, eh->no_zeros, "eh->no_zeros", "ehist_nozeroes");
}



#endif
