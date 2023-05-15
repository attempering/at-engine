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
    fprintf(stderr, "null pointer to at_mb_t\n");
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
  if (0 != zcom_cfg__get(cfg, &mb->need_cv, "mbest_needcv", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->need_cv = 1, key: mbest_needcv\n");
  }

  /* use symmetric windows */
  mb->use_sym_wins = 1;
  if (0 != zcom_cfg__get(cfg, &mb->use_sym_wins, "mbest_sym_mbin", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->use_sym_wins = 1, key: mbest_sym_mbin\n");
  }

  /* force the single bin estimator */
  mb->use_single_bin = 0;
  if (0 != zcom_cfg__get(cfg, &mb->use_single_bin, "mbest_single_bin", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->use_single_bin = 0, key: mbest_single_bin\n");
  }

  /* being verbose */
  mb->verbose = 1;
  if (0 != zcom_cfg__get(cfg, &mb->verbose, "mbest_verbose", "%u")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->verbose = 1, key: mbest_verbose\n");
  }

  at_mb_win__cfg_init(mb->win, cfg, mb);

  /* nst_refresh: interval of recalculating et for all temperatures */
  mb->nst_refresh = 10000;
  if (0 != zcom_cfg__get(cfg, &mb->nst_refresh, "nstrefresh", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->nstrefresh = 10000, key: nstrefresh\n");
  }

  /* nst_save_av: interval of writing mbav and ze files */
  mb->nst_save_av = 10000;
  if (0 != zcom_cfg__get(cfg, &mb->nst_save_av, "nstav", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->nst_save_av = 10000, key: nstav\n");
  }

  /* av_binary: use binary format in mbav file */
  mb->av_binary = AT__TRUE;
  if (0 != zcom_cfg__get(cfg, &mb->av_binary, "mbav_binary", "%d")) {
    IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->av_binary = 1, key: mbav_binary\n");
  }

  /* av_file: name of mbav file */
  {
    char *fn_mb = "mb.dat";
    if (0 != zcom_cfg__get(cfg, &mb->av_file, "mbav_file", "%s")) {
      IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->av_file = \"%s\", key: mbav_file\n", fn_mb);
    }
    mb->av_file = at_utils__make_output_filename(ssm, data_dir, fn_mb);
  }

  /* ze_file: name of ze file */
  {
    char *fn_ze = "ze.dat";
    if (0 != zcom_cfg__get(cfg, &mb->ze_file, "ze_file", "%s")) {
      IF_VERBOSE_FPRINTF(stderr, "Info@at: assuming default mb->ze_file = \"%s\", key: ze_file\n", fn_ze);
    }
    mb->ze_file = at_utils__make_output_filename(ssm, data_dir, fn_ze);

    mb->ze_init_file = at_utils__make_output_filename(ssm, data_dir, "ze-init.dat");
  }

  /* visits: number of visits */
  if ((mb->visits = (double *) calloc((n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "at->error: no memory! var: mb->visits, type: double\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
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
  /* compute heat capacity */
  at_utils_manifest__print_bool(manifest, mb->need_cv, "mb->need_cv", "mbest_needcv");

  /* use symmetrical window */
  at_utils_manifest__print_bool(manifest, mb->use_sym_wins, "mb->use_sym_wins", "mbest_needcv");

  /* use single bin estimator */
  at_utils_manifest__print_bool(manifest, mb->use_single_bin, "mb->use_single_bin", "mbest_needcv");

  /* being verbose */
  at_utils_manifest__print_bool(manifest, mb->verbose, "mb->verbose", "mbest_verbose");

  at_mb_win__manifest(mb->win, manifest);

  /* interval of recalculating et for all temperature */
  at_utils_manifest__print_int(manifest, mb->nst_refresh, "mb->nst_refresh", "nstrefresh");

  /* interval of writing mbav and ze files */
  at_utils_manifest__print_int(manifest, mb->nst_save_av, "mb->nst_save_av", "nstav");

  /* use binary format in mbav file */
  at_utils_manifest__print_bool(manifest, mb->av_binary, "mb->av_binary", "mbav_binary");

  /* name of mbav file */
  at_utils_manifest__print_str(manifest, mb->av_file, "mb->av_file", "mbav_file");

  /* name of ze file */
  at_utils_manifest__print_str(manifest, mb->ze_file, "mb->ze_file", "ze_file");

  /* number of visits */
  at_utils_manifest__print_double_arr(manifest, mb->visits, mb->distr->domain->n, "mb->visits");

  /* total number of visits, number of tempering */
  at_utils_manifest__print_double(manifest, mb->total_visits, "mb->total_visits", NULL);

  at_mb_shk__manifest(mb->shk, manifest);

  at_mb_iie__manifest(mb->iie, manifest);

  at_mb_accum__manifest(mb->accum, manifest);

  /* number of additional integer variables to be written to binary file */
  at_utils_manifest__print_int(manifest, mb->cnt_int, "mb->cnt_int", NULL);

  /* number of additional double variables to be written to binary file */
  at_utils_manifest__print_int(manifest, mb->cnt_dbl, "mb->cnt_dbl", NULL);

}



#endif
