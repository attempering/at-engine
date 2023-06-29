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

#ifndef AT_MB_BASIC__SRC_H__
#define AT_MB_BASIC__SRC_H__

/* basic routines for the multiple-bin estimators */

#include "at_mb_basic.h"

/* adaptive averaging functions */
#include "shk/at_mb_shk.h"

#include "iie/at_mb_iie.h"

#include "accum/at_mb_accum.h"


#include "../../zcom/zcom.h"
#include "../utils/at_utils.h"




#define IF_VERBOSE_FPRINTF  if(verbose) fprintf

int at_mb__cfg_init(
    at_mb_t *mb,
    at_distr_t *distr,
    zcom_cfg_t *cfg,
    double boltz,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  int i, n;

  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb: null pointer to at_mb_t\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  mb->distr = distr;

  n = distr->domain->n;
  //mb->n = n;

  mb->boltz = boltz;

  //printf("mb->flags %u\n", mb->flags); getchar();

  /* compute heat capacity */
  mb->need_cv = 1;
  if (0 != zcom_cfg__get(cfg, &mb->need_cv, "mbest_needcv,mb-need-cv", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->need_cv = 1, key: mb-need-cv\n");
  }

  /* use symmetric windows */
  mb->use_sym_wins = 1;
  if (0 != zcom_cfg__get(cfg, &mb->use_sym_wins, "mbest_sym_mbin,mb-use-sym-wins", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->use_sym_wins = 1, key: mb-use-sym-wins\n");
  }

  /* force the single bin estimator */
  mb->use_single_bin = 0;
  if (0 != zcom_cfg__get(cfg, &mb->use_single_bin, "mbest_single_bin,mb-use-single-bin", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->use_single_bin = 0, key: mb-use-single-bin\n");
  }

  /* being verbose */
  mb->verbose = 1;
  if (0 != zcom_cfg__get(cfg, &mb->verbose, "mbest-verbose", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->verbose = 1, key: mbest-verbose\n");
  }

  at_mb_win__cfg_init(mb->win, cfg, mb);

  /* nst_refresh: interval of recalculating et for all temperatures */
  mb->nst_refresh = 10000;
  if (0 != zcom_cfg__get(cfg, &mb->nst_refresh, "nstrefresh,nst-refresh,mb-nst-refresh", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->nst_refresh = %d, key: mb-nst-refresh\n",
        mb->nst_refresh);
  }

  /* nst_save_av: interval of writing mbav and ze files */
  mb->nst_save_av = 10000;
  if (0 != zcom_cfg__get(cfg, &mb->nst_save_av, "nstav,nst-av,mb-nst-save", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->nst_save_av = 10000, key: mb-nst-save\n");
  }

  /* use binary format in average file */
  mb->use_binary_file = AT__TRUE;
  /*
  if (0 != zcom_cfg__get(cfg, &mb->use_binary_file, "mbav-binary,mb-use-binary-file", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->use_binary_file = 1, key: mb-use-binary-file\n");
  }
  */

  /* names of the average data file */
  {
    char *fn_binary = zcom_ssm__dup(ssm, "mb.dat");
    if (0 != zcom_cfg__get(cfg, &fn_binary, "mbav-file,mb-file,mb-binary-file", "%s")) {
      IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->file_binary = [%s], key: mb-binary-file\n", fn_binary);
    }
    mb->file_binary = at_utils__make_output_filename(ssm, data_dir, fn_binary);

    char *fn_text = zcom_ssm__dup(ssm, "mb-text.dat");
    if (0 != zcom_cfg__get(cfg, &fn_text, "mb-text-file", "%s")) {
      IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->file_text = [%s], key: mb-text-file\n",
        fn_text);
    }
    mb->file_text = at_utils__make_output_filename(ssm, data_dir, fn_text);
  }

  /* ze_file: name of ze file */
  {
    char *fn_ze = zcom_ssm__dup(ssm, "ze.dat");
    if (0 != zcom_cfg__get(cfg, &mb->ze_file, "ze-file", "%s")) {
      IF_VERBOSE_FPRINTF(stderr, "Info@at.mb: assuming default mb->ze_file = [%s], key: ze-file\n", fn_ze);
    }
    mb->ze_file = at_utils__make_output_filename(ssm, data_dir, fn_ze);

    mb->ze_init_file = at_utils__make_output_filename(ssm, data_dir, "ze-init.dat");
  }

  /* visits: number of visits */
  if ((mb->visits = (double *) calloc((n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "Error@at.mb: no memory! var: mb->visits, type: double\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  for (i = 0; i < n; i++) {
    mb->visits[i] = 0.0;
  }

  /* total_visits: total number of visits, number of tempering */
  mb->total_visits = 0.0;

  at_mb_iie__cfg_init(mb->iie, mb, cfg, verbose);

  at_mb_accum__cfg_init(mb->accum, distr->domain->n, mb->win, cfg, verbose);

  at_mb_shk__cfg_init(mb->shk, cfg, mb, verbose);

  return 0;

ERR:

  return -1;

}

#undef IF_VERBOSE_FPRINTF



void at_mb__finish(at_mb_t *mb)
{
  if (mb->visits != NULL) {
    free(mb->visits);
  }

  at_mb_shk__finish(mb->shk);

  at_mb_accum__finish(mb->accum);

  at_mb_win__finish(mb->win);

  at_mb_iie__finish(mb->iie);

  memset(mb, 0, sizeof(*mb));
}



void at_mb__clear(at_mb_t *mb)
{
  at_mb_accum__clear(mb->accum);
  at_mb_iie__clear(mb->iie);
}


void at_mb__manifest(const at_mb_t *mb, at_utils_manifest_t *manifest)
{
  /* compute heat capacity */
  at_utils_manifest__print_bool(manifest, mb->need_cv, "mb->need_cv", "mb-need-cv");

  /* use symmetrical window */
  at_utils_manifest__print_bool(manifest, mb->use_sym_wins, "mb->use_sym_wins", "mb-use-sym-wins");

  /* use single bin estimator */
  at_utils_manifest__print_bool(manifest, mb->use_single_bin, "mb->use_single_bin", "mb-use-single-bin");

  /* being verbose */
  at_utils_manifest__print_bool(manifest, mb->verbose, "mb->verbose", "mb-verbose");

  at_mb_win__manifest(mb->win, manifest);

  /* interval of recalculating et for all temperature */
  at_utils_manifest__print_int(manifest, mb->nst_refresh, "mb->nst_refresh", "mb-nst-refresh");

  /* interval of writing mbav and ze files */
  at_utils_manifest__print_int(manifest, mb->nst_save_av, "mb->nst_save_av", "mb-nst-save");

  /* use binary format in mbav file */
  at_utils_manifest__print_bool(manifest, mb->use_binary_file, "mb->use_binary_file", "mb-use-binary-file");

  /* name of the average file */
  at_utils_manifest__print_str(manifest, mb->file_binary, "mb->file_binary", "mb-binary-file");
  at_utils_manifest__print_str(manifest, mb->file_text, "mb->file_text", "mb-text-file");

  /* name of ze file */
  at_utils_manifest__print_str(manifest, mb->ze_file, "mb->ze_file", "ze-file");

  /* number of visits */
  at_utils_manifest__print_double_arr(manifest, mb->visits, mb->distr->domain->n, "mb->visits");

  /* total number of visits, number of tempering */
  at_utils_manifest__print_double(manifest, mb->total_visits, "mb->total_visits", NULL);

  at_mb_shk__manifest(mb->shk, manifest);

  at_mb_iie__manifest(mb->iie, manifest);

  at_mb_accum__manifest(mb->accum, manifest);
}



#endif
