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


#include "../../zcom/zcom.h"
#include "../utils/at_utils.h"




#define IF_VERBOSE_FPRINTF  if(!silent) fprintf

int at_mb__cfg_init(
    at_mb_t *mb,
    at_distr_t *distr,
    zcom_cfg_t *cfg,
    double boltz,
    zcom_ssm_t *ssm,
    const char *data_dir,
    int silent)
{
  int i, n;

  if (mb == NULL) {
    fprintf(stderr, "null pointer to at_mb_t\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  mb->distr = distr;

  n = distr->domain->n;
  mb->n = n;

  mb->boltz = boltz;

  /* flags: combination of flags */
  mb->flags = 0;

  /* MB_USE_WIN_ACCUM: use adaptive averaging */
  i = 1;
  if (0 != zcom_cfg__get(cfg, &i, "mbest_damp", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: MB_USE_WIN_ACCUM = 1, key: mbest_damp\n");
  }
  zcom_util__exit_if ( !(i == 0 || i == 1),
      "MB_USE_WIN_ACCUM: failed validation: i == 0 || i == 1\n");

  if (i) {
    mb->flags |= MB_USE_WIN_ACCUM;
  } else {
    mb->flags &= ~MB_USE_WIN_ACCUM;
  }

  //printf("mb->flags %u\n", mb->flags); getchar();

  /* MB_CV: compute heat capacity */
  i = 1;
  if (0 != zcom_cfg__get(cfg, &i, "mbest_needcv", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: MB_CV = 1, key: mbest_needcv\n");
  }
  zcom_util__exit_if ( !(i == 0 || i == 1),
      "MB_CV: failed validation: i == 0 || i == 1\n");
  if (i) {
    mb->flags |= MB_CV;
  } else {
    mb->flags &= ~MB_CV;
  }

  /* MB_SYMWIN: use symmetric windows */
  i = 1;
  if (0 != zcom_cfg__get(cfg, &i, "mbest_sym_mbin", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: MB_SYMWIN = 1, key: mbest_sym_mbin\n");
  }
  zcom_util__exit_if ( !(i == 0 || i == 1),
      "MB_SYMWIN: failed validation: i == 0 || i == 1\n");
  if (i) {
    mb->flags |= MB_SYMWIN;
  } else {
    mb->flags &= ~MB_SYMWIN;
  }

  /* MB_SINGLE_BIN: use single bin estimator */
  i = 0;
  if (0 != zcom_cfg__get(cfg, &i, "mbest_single_bin", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: MB_SINGLE_BIN = 0, key: mbest_single_bin\n");
  }
  zcom_util__exit_if ( !(i == 0 || i == 1),
      "MB_SINGLE_BIN: failed validation: i == 0 || i == 1\n");
  if (i) {
    mb->flags |= MB_SINGLE_BIN;
  } else {
    mb->flags &= ~MB_SINGLE_BIN;
  }

  /* MB_VERBOSE: being verbose */
  i = 1;
  if (0 != zcom_cfg__get(cfg, &i, "mbest_verbose", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: MB_VERBOSE = 1, key: mbest_verbose\n");
  }
  zcom_util__exit_if ( !(i == 0 || i == 1),
      "MB_VERBOSE: failed validation: i == 0 || i == 1\n");
  if (i) {
    mb->flags |= MB_VERBOSE;
  } else {
    mb->flags &= ~MB_VERBOSE;
  }

  at_mb_win__cfg_init(mb->win, cfg, mb);

  /* nstrefresh: interval of recalculating et for all temperatures */
  mb->nstrefresh = 10000;
  if (0 != zcom_cfg__get(cfg, &mb->nstrefresh, "nstrefresh", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: mb->nstrefresh = 10000, key: nstrefresh\n");
  }

  /* nst_save_av: interval of writing mbav and ze files */
  mb->nst_save_av = 10000;
  if (0 != zcom_cfg__get(cfg, &mb->nst_save_av, "nstav", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: mb->nst_save_av = 10000, key: nstav\n");
  }

  /* av_binary: use binary format in mbav file */
  mb->av_binary = 1;
  if (0 != zcom_cfg__get(cfg, &mb->av_binary, "mbav_binary", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: mb->av_binary = 1, key: mbav_binary\n");
  }

  /* av_file: name of mbav file */
  {
    char *fn_mb = "mb.dat";
    if (0 != zcom_cfg__get(cfg, &mb->av_file, "mbav_file", "%s")) {
      IF_VERBOSE_FPRINTF(stderr, "assuming default: mb->av_file = \"%s\", key: mbav_file\n", fn_mb);
    }
    mb->av_file = at_utils__make_output_filename(ssm, data_dir, fn_mb);
  }

  /* ze_file: name of ze file */
  {
    char *fn_ze = "ze.dat";
    if (0 != zcom_cfg__get(cfg, &mb->ze_file, "ze_file", "%s")) {
      IF_VERBOSE_FPRINTF(stderr, "assuming default: mb->ze_file = \"%s\", key: ze_file\n", fn_ze);
    }
    mb->ze_file = at_utils__make_output_filename(ssm, data_dir, fn_ze);
  }

  /* wze_reps: number of iterations before writing ze file */
  mb->wze_reps = 5;
  if (0 != zcom_cfg__get(cfg, &mb->wze_reps, "mbest_wze_reps", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "assuming default: mb->wze_reps = 5, key: mbest_wze_reps\n");
  }

  /* visits: number of visits */
  if ((mb->visits = (double *) calloc((n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "no memory! var: mb->visits, type: double\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  for (i = 0; i < n; i++) {
    mb->visits[i] = 0.0;
  }

  /* total_visits: total number of visits, number of tempering */
  mb->total_visits = 0.0;

  /* cnt_int: number of additional integer variables to be written to binary file */
  mb->cnt_int = 0;
  /* cnt_dbl: number of additional double variables to be written to binary file */
  mb->cnt_dbl = 5;

  at_mb_iie__cfg_init(mb->iie, mb, cfg, silent);

  at_mb_accum__init(mb->accum, distr->domain->n, mb->win, mb->flags);

  at_mb_shk__cfg_init(mb->shk, cfg, mb, mb->accum->m, silent);

  return 0;

ERR:

  return -1;

}

#undef IF_VERBOSE_FPRINTF



void at_mb__finish(at_mb_t *mb)
{
  if (mb->visits    != NULL) free(mb->visits);

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
  FILE *fp = manifest->fp;

  /* flags: combination of flags */
  fprintf(fp, "mb->flags: unsigned, 0x%X\n", mb->flags);

  /* MB_USE_WIN_ACCUM: use adaptive averaging */
  fprintf(fp, "mb->flags & MB_USE_WIN_ACCUM (0x%X, mbest_damp): %s\n",
    MB_USE_WIN_ACCUM, (mb->flags & MB_USE_WIN_ACCUM) ? "on" : "off");

  /* MB_CV: compute heat capacity */
  fprintf(fp, "mb->flags & MB_CV (0x%X, mbest_needcv): %s\n",
    MB_CV, (mb->flags & MB_CV) ? "on" : "off");

  /* MB_SYMWIN: use symmetrical window */
  fprintf(fp, "mb->flags & MB_SYMWIN (0x%X, mbest_sym_mbin): %s\n",
    MB_SYMWIN, (mb->flags & MB_SYMWIN) ? "on" : "off");

  /* MB_SINGLE_BIN: use single bin estimator */
  fprintf(fp, "mb->flags & MB_SINGLE_BIN (0x%X, mbest_single_bin): %s\n",
    MB_SINGLE_BIN, (mb->flags & MB_SINGLE_BIN) ? "on" : "off");

  /* MB_VERBOSE: being verbose */
  fprintf(fp, "mb->flags & MB_VERBOSE (0x%X, mbest_verbose): %s\n",
    MB_VERBOSE, (mb->flags & MB_VERBOSE) ? "on" : "off");

  at_mb_win__manifest(mb->win, manifest);

  /* nstrefresh: interval of recalculating et for all temperature */
  fprintf(fp, "mb->nstrefresh: int, %4d\n", mb->nstrefresh);

  /* nst_save_av: interval of writing mbav and ze files */
  fprintf(fp, "mb->nst_save_av: int, %4d\n", mb->nst_save_av);

  /* av_binary: use binary format in mbav file */
  fprintf(fp, "mb->av_binary: int, %4d\n", mb->av_binary);

  /* av_file: name of mbav file */
  fprintf(fp, "mb->av_file: char *, %s\n", mb->av_file);

  /* ze_file: name of ze file */
  fprintf(fp, "mb->ze_file: char *, %s\n", mb->ze_file);

  /* wze_reps: number of iterations before writing ze file */
  fprintf(fp, "mb->wze_reps: int, %4d\n", mb->wze_reps);

  /* visits: number of visits */
  at_utils_manifest__print_double_arr(manifest, mb->visits, mb->distr->domain->n, "mb->visits");

  /* total_visits: total number of visits, number of tempering */
  fprintf(fp, "mb->total_visits: double, %g\n", mb->total_visits);

  at_mb_shk__manifest(mb->shk, manifest);

  at_mb_iie__manifest(mb->iie, manifest);

  at_mb_accum__manifest(mb->accum, manifest);

  /* cnt_int: number of additional integer variables to be written to binary file */
  fprintf(fp, "mb->cnt_int: int, %4d\n", mb->cnt_int);

  /* cnt_dbl: number of additional double variables to be written to binary file */
  fprintf(fp, "mb->cnt_dbl: int, %4d\n", mb->cnt_dbl);
}



#endif
