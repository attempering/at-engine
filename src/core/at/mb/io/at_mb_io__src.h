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

#ifndef AT_MB_IO__SRC_H__
#define AT_MB_IO__SRC_H__



/* IO routines for at_mb_t */

#include "at_mb_io.h"

/* implementation dependent headers */
#include "../at_mb_basic.h"

/* include the source code of sub-modules */
#include "at_mb_io_binary__src.h"




int at_mb__read(at_mb_t *mb, at_langevin_t *langevin, double *beta)
{
  int ret, version;

  ret = at_mb__read_binary(mb, langevin, beta, mb->av_file, &version);

  if (ret != 0) {
    fprintf(stderr, "cannot load at_mb_t data from %s\n", mb->av_file);
    return 1;
  }

  fprintf(stderr, "loaded previous at_mb_t data, %s version: %d\n",
    mb->av_file, version);

  return 0;
}



int at_mb__write(at_mb_t *mb, at_langevin_t *langevin, double beta)
{
  return at_mb__write_binary(mb, langevin, beta, mb->av_file, 1);
}



void at_mb__manifest(const at_mb_t *mb, at_utils_manifest_t *manifest)
{
  FILE *fp = manifest->fp;

  /* bdel: bin size of beta */
  fprintf(fp, "mb->bdel: double, %g\n", mb->bdel);

  /* n: number of temperature bins */
  fprintf(fp, "mb->n: int, %4d\n", mb->n);

  /* barr: temperature array */
  at_utils_manifest__print_double_arr(manifest, mb->barr, mb->n+1, "mb->barr");

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
  at_utils_manifest__print_double_arr(manifest, mb->visits, mb->n, "mb->visits");

  /* total_visits: total number of visits, number of tempering */
  fprintf(fp, "mb->total_visits: double, %g\n", mb->total_visits);

  at_mb_betadist__manifest(mb->betadist, manifest);

  at_mb_shk__manifest(mb->shk, manifest);

  at_mb_iie__manifest(mb->iie, manifest);

  at_mb_accum__manifest(mb->accum, manifest);

  /* cnt_int: number of additional integer variables to be written to binary file */
  fprintf(fp, "mb->cnt_int: int, %4d\n", mb->cnt_int);

  /* cnt_dbl: number of additional double variables to be written to binary file */
  fprintf(fp, "mb->cnt_dbl: int, %4d\n", mb->cnt_dbl);
}

#endif
