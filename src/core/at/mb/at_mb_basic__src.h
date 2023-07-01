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



int at_mb__conf_init(
    at_mb_t *mb,
    at_distr_t *distr,
    at_utils_conf_t *conf,
    double boltz)
{
  int i, n;

  at_utils_conf__push_mod(conf, "at.mb");

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

  /* whether to compute the heat capacity */
  at_utils_conf__get_bool(conf,
      "mbest_needcv,mb-need-cv",
      &mb->need_cv, AT__TRUE,
      "need_cv");

  /* use symmetric windows */
  at_utils_conf__get_bool(conf,
      "mbest_sym_mbin,mb-use-sym-wins",
      &mb->use_sym_wins, AT__TRUE,
      "use_sym_wins");

  /* force the single bin estimator */
  at_utils_conf__get_bool(conf,
      "mbest_single_bin,mb-use-single-bin",
      &mb->use_single_bin, AT__FALSE,
      "use_single_bin");

  /* being verbose */
  at_utils_conf__get_bool(conf,
      "mbest_verbose,mb-verbose",
      &mb->verbose, AT__TRUE,
      "verbose");

  at_mb_win__cfg_init(mb->win, conf->cfg, mb);

  /* nst_refresh: interval of recalculating et for all temperatures */
  at_utils_conf__get_int(conf,
      "nstrefresh,nst-refresh,mb-nst-refresh",
      &mb->nst_refresh, 10000,
      "nst_refresh");

  /* nst_save_av: interval of writing mbav and ze files */
  at_utils_conf__get_int(conf,
      "nstav,nst-av,mb-nst-save",
      &mb->nst_save_av, 10000,
      "nst_save_av");

  at_utils_conf__get_bool(conf,
      "mb-use-text-file",
      &mb->use_text_file, AT__FALSE,
      "use_text_file");

  /* names of the average data file */
  at_utils_conf__get_filename(conf,
      "mbav-file,mb-file,mb-binary-file",
      &mb->file_binary, "mb.dat",
      "file_binary");

  at_utils_conf__get_filename(conf,
      "mb-text-file",
      &mb->file_text, "mb-text.dat",
      "file_text");

  /* ze_file: name of ze file */
  at_utils_conf__get_filename(conf,
      "ze-file",
      &mb->ze_file, "ze.dat",
      "ze_file");

  at_utils_conf__get_filename(conf,
      "ze-init-file",
      &mb->ze_init_file, "ze-init.dat",
      "ze_init_file");

  /* visits: number of visits */
  mb->visits = (double *) calloc(n, sizeof(double));
  at_utils_log__exit_if (mb->visits == NULL,
      conf->log,
      "no memory for mb->visits of size %d, type: double\n",
      n);
  for (i = 0; i < n; i++) {
    mb->visits[i] = 0.0;
  }

  /* total_visits: total number of visits, number of tempering */
  mb->total_visits = 0.0;

  at_mb_iie__conf_init(mb->iie, mb, conf);

  at_mb_accum__conf_init(mb->accum, distr->domain->n, mb->win, conf);

  at_mb_shk__conf_init(mb->shk, mb, conf);

  at_utils_conf__pop_mod(conf);

  return 0;

ERR:

  return -1;

}



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
  at_utils_manifest__push_mod(manifest, "at.mb");

  /* compute heat capacity */
  at_utils_manifest__print_bool(manifest, mb->need_cv, "need_cv", "mb-need-cv");

  /* use symmetrical window */
  at_utils_manifest__print_bool(manifest, mb->use_sym_wins, "use_sym_wins", "mb-use-sym-wins");

  /* use single bin estimator */
  at_utils_manifest__print_bool(manifest, mb->use_single_bin, "use_single_bin", "mb-use-single-bin");

  /* being verbose */
  at_utils_manifest__print_bool(manifest, mb->verbose, "verbose", "mb-verbose");

  /* interval of recalculating et for all temperature */
  at_utils_manifest__print_int(manifest, mb->nst_refresh, "nst_refresh", "mb-nst-refresh");

  /* interval of writing mbav and ze files */
  at_utils_manifest__print_int(manifest, mb->nst_save_av, "nst_save_av", "mb-nst-save");

  /* use binary format in mbav file */
  at_utils_manifest__print_bool(manifest, mb->use_text_file, "use_text_file", "mb-use-text-file");

  /* name of the average file */
  at_utils_manifest__print_str(manifest, mb->file_binary, "file_binary", "mb-binary-file");
  at_utils_manifest__print_str(manifest, mb->file_text, "file_text", "mb-text-file");

  /* name of ze file */
  at_utils_manifest__print_str(manifest, mb->ze_file, "ze_file", "ze-file");

  /* number of visits */
  at_utils_manifest__print_double_arr(manifest, mb->visits, mb->distr->domain->n, "visits");

  /* total number of visits, number of tempering */
  at_utils_manifest__print_double(manifest, mb->total_visits, "total_visits", NULL);

  at_utils_manifest__pop_mod(manifest);

  at_mb_win__manifest(mb->win, manifest);

  at_mb_shk__manifest(mb->shk, manifest);

  at_mb_iie__manifest(mb->iie, manifest);

  at_mb_accum__manifest(mb->accum, manifest);
}



#endif
