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

int at_eh__cfg_init(at_eh_t *eh, at_mb_t *mb, zcom_cfg_t *cfg,
    zcom_ssm_t *ssm, const char *data_dir, at_bool_t verbose)
{
  int i;
  at_distr_t *distr = mb->distr;
  at_distr_domain_t *domain = distr->domain;

  eh->mb = mb;
  eh->n = distr->domain->n;

  eh->e_min_runtime = 0;
  eh->e_max_runtime = 0;

  /* eh_mode: 0: disable; 1: simple histogram */
  eh->mode = 0;
  if (0 != zcom_cfg__get(cfg, &eh->mode, "ehist-mode", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->mode = 0, key: ehist-mode\n");
  }
  if ( !(eh->mode == 0 || eh->mode == 1) ) {
    fprintf(stderr, "eh->mode: failed validation: eh->mode == 0 || eh->mode == 1\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* eh_skip: interval of reconstructing energy histograms */
  eh->recon_stride = 10;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->recon_stride, "ehist-skip,ehist-recon-stride", "%d")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->recon_stride = 10, key: ehist-recon-stride\n");
    }
    if ( !(eh->recon_stride > 0) ) {
      fprintf(stderr, "eh->recon_stride: failed validation: eh->recon_stride > 0\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  eh->bwmod = 1;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->bwmod, "ehist-recon-mode,ehist_mbin_mode", "%d")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->bwmod = 1, key: ehist-recon-mode\n");
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
    if (0 != zcom_cfg__get(cfg, &eh->bwdel, "ehist-del-lnT", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->bwdel = %g, key: ehist-del-lnT\n", eh->bwdel);
    }
    if ( !(eh->bwdel > domain->beta_del/domain->beta_min) ) {
      fprintf(stderr, "eh->bwdel: failed validation: eh->bwdel %g > (beta-del %g)/(beta-min %g)\n",
          eh->bwdel, domain->beta_del, domain->beta_min);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwdel: delta beta */
  if (eh->mode && eh->bwmod == 0) {
    eh->bwdel = 0.02;
    if (0 != zcom_cfg__get(cfg, &eh->bwdel, "ehist-del-beta", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->bwdel = %g, key: ehist-del-beta\n", eh->bwdel);
    }
    if ( !(eh->bwdel > distr->domain->beta_del) ) {
      fprintf(stderr, "eh->bwdel: failed validation: (eh->bwdel %g) > (beta-del %g)\n",
          eh->bwdel, domain->beta_del);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwdel: delta kT */
  if (eh->mode && eh->bwmod == 2) {
    eh->bwdel = 0.10;
    if (0 != zcom_cfg__get(cfg, &eh->bwdel, "ehist-del-kT", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->bwdel = %g, key: ehist-del-kT\n", eh->bwdel);
    }
    if ( !(eh->bwdel > distr->domain->beta_del/pow(distr->domain->beta_min, 2.0)) ) {
      fprintf(stderr, "eh->bwdel: failed validation: (eh->bwdel %g) > (beta-del %g)/(beta-min %g)^2\n",
          eh->bwdel, domain->beta_del, domain->beta_min);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_min: minimal energy */
  eh->e_min = -12.6e4;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->e_min, "ehist-min,ehist-e-min", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->e_min = -12.6e4, key: ehist-e-min\n");
    }
  }
  eh->e_min_runtime = eh->e_min;

  /* eh_max: maximal energy */
  eh->e_max = -9.0e4;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->e_max, "ehist-max,ehist-e-max", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->e_max = -9.0e4, key: ehist-e-max\n");
    }
    if ( !(eh->e_max > eh->e_min) ) {
      fprintf(stderr, "eh->e_max: failed validation: eh->e_max > eh->e_min\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }
  eh->e_max_runtime = eh->e_max;

  /* eh_del: energy bin size */
  eh->e_del = 20.0;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->e_del, "ehist-del,ehist-e-del", "%lf")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->e_del = 20.0, key: ehist-e-del\n");
    }
    if ( !(eh->e_del > 0) ) {
      fprintf(stderr, "eh->e_del: failed validation: eh->e_del > 0\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_cnt: number of energy bins */
  eh->e_n = (int)((eh->e_max-eh->e_min)/eh->e_del - 1e-5 + 1);

  eh->use_text_file = AT__FALSE;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->use_text_file, "ehist-use-text-file", "%d")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->use_text_file = %d, key: ehist-use-text-file\n",
          eh->use_text_file);
    }
  }

  /* eh_nst_save: interval of writing histogrm files */
  eh->nst_save = 100000;
  if (eh->mode) {
    if (0 != zcom_cfg__get(cfg, &eh->nst_save, "nsthist,nst-hist,ehist-nst-save", "%d")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->nst_save = 100000, key: ehist-nst-save\n");
    }
  }

  if (eh->mode) {

    {
      eh->file_binary = NULL;
      char *fn_eh_binary = zcom_ssm__dup(ssm, "hist.dat");
      if (0 != zcom_cfg__get(cfg, &fn_eh_binary, "ehist-file,ehist-file-binary", "%s")) {
        if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->file_binary = [%s], key: ehist-file-binary\n", fn_eh_binary);
      }
      eh->file_binary = at_utils__make_output_filename(ssm, data_dir, fn_eh_binary);
    }

    {
      eh->file_text = NULL;
      char *fn_eh_text = zcom_ssm__dup(ssm, "hist-text.dat");
      if (0 != zcom_cfg__get(cfg, &fn_eh_text, "ehist-file-text", "%s")) {
        if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->file_text = [%s], key: ehist-file-text\n", fn_eh_text);
      }
      eh->file_text = at_utils__make_output_filename(ssm, data_dir, fn_eh_text);
    }

  }


  /* name of reconstructed energy histogram */
  eh->file_recon = NULL;
  if (eh->mode) {
    char *fn_eh_recon = zcom_ssm__dup(ssm, "hist-recon.dat");
    if (0 != zcom_cfg__get(cfg, &fn_eh_recon, "ehist-recon-file,ehist_mbin_file", "%s")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->file_recon = [%s], key: ehist-recon-file\n", fn_eh_recon);
    }
    eh->file_recon = at_utils__make_output_filename(ssm, data_dir, fn_eh_recon);
  }

  /* eh_his: energy histogram data */
  eh->his = NULL;
  if (eh->mode) {
    if ((eh->his = (double *) calloc((eh->n*eh->e_n + 1), sizeof(double))) == NULL) {
      fprintf(stderr, "Error@at.eh: no memory! var: eh->his, type: double\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->n*eh->e_n; i++)
      eh->his[i] = 0.0;
  }
  /* eh_recon: temporary space for reconstructing histogram */
  eh->recon = NULL;
  if (eh->mode) {
    if ((eh->recon = (double *) calloc((eh->e_n + 1), sizeof(double))) == NULL) {
      fprintf(stderr, "Error@at.eh: no memory! var: eh->recon, type: double\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->e_n; i++)
      eh->recon[i] = 0.0;
  }
  /* eh_is: indices for temperature windows (lower) */
  eh->is = NULL;
  if (eh->mode) {
    if ((eh->is = (int *) calloc((eh->n + 2), sizeof(int))) == NULL) {
      fprintf(stderr, "Error@at.eh: no memory! var: eh->is, type: int\n");
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
      fprintf(stderr, "Error@at.eh: no memory! var: eh->it, type: int\n");
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
    if (0 != zcom_cfg__get(cfg, &eh->add_half_ebin, "ehist-addahalf,ehist-add-half-ebin", "%u")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->add_a_half = 1, key: ehist-add-half-ebin\n");
    }

    /* keep zero margins */
    eh->keep_margins = AT__FALSE;
    if (0 != zcom_cfg__get(cfg, &eh->keep_margins, "ehist-keepedge,ehist-keep-margin,ehist-keep-margins", "%u")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->keep_margins = 0, key: ehist-keep-margins\n");
    }

    /* do not output zeroes */
    eh->no_zeros = 0;
    if (0 != zcom_cfg__get(cfg, &eh->no_zeros, "ehist-nozeroes,ehist-no-zeroes,ehist-no-zeros", "%u")) {
      if (verbose) fprintf(stderr, "Info@at.eh: assuming default eh->no_zeros = 0, key: ehist-no-zeros\n");
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
    for (i = 0; i < eh->n*eh->e_n; i++) {
      eh->his[i] = 0.0;
    }

    for (i = 0; i < eh->e_n; i++) {
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
  at_utils_manifest__push_mod(manifest, "at.eh");

  /* eh_mode: 0: disable; 1: simple histogram */
  at_utils_manifest__print_int(manifest, eh->mode, "mode", "ehist-mode");

  if (eh->mode == 0) {
    return;
  }

  /* interval of reconstructing energy histograms */
  at_utils_manifest__print_int(manifest, eh->recon_stride, "recon_stride", "ehist-recon-stride");

  /* eh_bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  at_utils_manifest__print_double(manifest, eh->bwmod, "bwmod", "ehist-recon-mode");

  if (eh->bwmod == 0) {
    at_utils_manifest__print_double(manifest, eh->bwdel, "bwdel", "ehist-del-beta");
  } else if (eh->bwmod == 1) {
    at_utils_manifest__print_double(manifest, eh->bwdel, "bwdel", "ehist-del-lnT");
  } else if (eh->bwmod == 2) {
    at_utils_manifest__print_double(manifest, eh->bwdel, "bwdel", "ehist-del-kT");
  }

  /* eh_min: minimal energy */
  at_utils_manifest__print_double(manifest, eh->e_min, "e_min", "ehist-e-min");

  /* eh_max: maximal energy */
  at_utils_manifest__print_double(manifest, eh->e_max, "e_max", "ehist-e-max");

  /* eh_del: energy bin size */
  at_utils_manifest__print_double(manifest, eh->e_del, "e_del", "ehist-e-del");

  /* eh_cnt: number of energy bins */
  at_utils_manifest__print_int(manifest, eh->e_n, "e_n", NULL);

  at_utils_manifest__print_bool(manifest, eh->use_text_file, "use_text_file", "ehist-use-text-file");

  /* eh_nst_save: interval of writing histogram files */
  at_utils_manifest__print_int(manifest, eh->nst_save, "nst_save", "ehist-nst-save");

  /* name of ehist files */
  at_utils_manifest__print_str(manifest, eh->file_binary, "file_binary", "ehist-file-binary");
  at_utils_manifest__print_str(manifest, eh->file_text, "file_text", "ehist-file-text");

  /* name of reconstructed energy histogram */
  at_utils_manifest__print_str(manifest, eh->file_recon, "file_recon", "ehist-recon-file");

  /* eh_his: energy histogram data */
  at_utils_manifest__print_double_arr(manifest, eh->his, eh->n*eh->e_n, "his");

  /* eh_recon: temporary space for reconstructing histogram */
  at_utils_manifest__print_double_arr(manifest, eh->recon, eh->e_n, "recon");

  /* eh_is: indices for temperature windows (lower) */
  at_utils_manifest__print_int_arr(manifest, eh->is, eh->n + 1, "is");

  /* eh_it: indices for temperature windows (higher) */
  at_utils_manifest__print_int_arr(manifest, eh->it, eh->n + 1, "it");

  /* whether to add a half energy bin width in output */
  at_utils_manifest__print_bool(manifest, eh->add_half_ebin, "add_half_ebin", "ehist-add-half-ebin");

  /* whether to keep empty margin */
  at_utils_manifest__print_bool(manifest, eh->keep_margins, "keep_margins", "ehist-keep-margins");

  /* whether to include empty energy bins in the output */
  at_utils_manifest__print_bool(manifest, eh->no_zeros, "no_zeros", "ehist-no-zeros");

  at_utils_manifest__pop_mod(manifest);
}



#endif
