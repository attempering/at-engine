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

/* binary IO routines for at_mb_t */
#ifndef AT_MB_IO_BINARY_V3__SRC_H__
#define AT_MB_IO_BINARY_V3__SRC_H__

#include "at_mb_io_binary_v3.h"

/* implementation headers */
#include "../../../at_mb_basic.h"
#include "../../../sm/at_mb_sm.h"
#include "../../../shk/at_mb_shk.h"
#include "../../../accum/at_mb_accum.h"
#include "../../../../distr/at_distr.h"

#include "../../../../../zcom/zcom.h"



int at_mb__read_binary_v3_low_level(
    at_mb_t *mb,
    const char *fn,
    FILE *fp,
    int endn)
{
  const int version = 3;
  at_utils_io_t io[1];

  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: passing null pointer to at_mb__read_binary_low_level\n");
    return -1;
  }

  /* clear data before reading */
  at_mb__clear(mb);

  at_utils_io_binary__init_read(io, "at.mb.io.binary.v3", fn, fp, endn);

  /* n: number of temperature bins */
  if (at_utils_io_binary__read_and_compare_int(io, NULL, "n", mb->distr->domain->n) != 0) {
    goto ERR;
  }

  /* beta_min */
  if (at_utils_io_binary__read_and_compare_double(io, NULL, "beta_min", mb->distr->domain->beta_min, 1e-8) != 0) {
    goto ERR;
  }

  /* beta_max */
  if (at_utils_io_binary__read_and_compare_double(io, NULL, "beta_max", mb->distr->domain->beta_max, 1e-8) != 0) {
    goto ERR;
  }

  /* m: maximal number of bins in a window */
  if (at_utils_io_binary__read_and_compare_int(io, NULL, "m", mb->win->max_win_bins) != 0) {
    goto ERR;
  }

  /* total_visits: total number of visits, number of tempering steps */
  if (at_utils_io_binary__read_double(io, &mb->total_visits,
      "total_visits", AT_UTILS_IO__NONNEGATIVE) != 0) {
    goto ERR;
  }

  /* visits: number of visits to bins */
  if (at_utils_io_binary__read_double_array(io, mb->distr->domain->n, mb->visits,
      "mb->visits", AT_UTILS_IO__NONNEGATIVE) != 0) {
    goto ERR;
  }

  if (at_mb_shk__read_binary(mb->shk, io) != 0) {
    goto ERR;
  }

  if (at_mb_accum__read_binary(mb->accum, io) != 0) {
    goto ERR;
  }

  return 0;

ERR:

  at_mb__clear(mb);

  return -1;
}



int at_mb__write_binary_v3_low_level(
    at_mb_t *mb,
    const char *fn,
    FILE *fp)
{
  const int version = 3;
  at_utils_io_t io[1];

  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: passing null pointer to at_mb__write_binary_low_level\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  at_utils_io_binary__init_write(io, "at.mb.io.binary.v3", fn, fp);

  if (zcom_endn__fwrite(&mb->distr->domain->n, sizeof(mb->distr->domain->n), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: error in writing mb->distr->domain->n\n");
    goto ERR;
  }

  if (zcom_endn__fwrite(&mb->distr->domain->beta_min, sizeof(mb->distr->domain->beta_min), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: error in writing mb->distr->domain->beta_min\n");
    goto ERR;
  }

  if (zcom_endn__fwrite(&mb->distr->domain->beta_max, sizeof(mb->distr->domain->beta_max), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: error in writing mb->distr->domain->beta_max\n");
    goto ERR;
  }

  /* m: maximal number of bins in a window */
  if (zcom_endn__fwrite(&mb->win->max_win_bins, sizeof(mb->win->max_win_bins), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: error in writing mb->win->max_win_bins\n");
    goto ERR;
  }

  /* total_visits: total number of visits, number of tempering steps */
  if (zcom_endn__fwrite(&mb->total_visits, sizeof(mb->total_visits), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: error in writing mb->total_visits\n");
    goto ERR;
  }

  /* visits: number of visits */
  if (zcom_endn__fwrite(mb->visits, sizeof(mb->visits[0]), mb->distr->domain->n, fp, 1) != mb->distr->domain->n) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: error in writing mb->visits\n");
    goto ERR;
  }

  if (at_mb_shk__write_binary(mb->shk, io) != 0) {
    goto ERR;
  }

  if (at_mb_accum__write_binary(mb->accum, io) != 0) {
    goto ERR;
  }

  return 0;
ERR:
  return -1;
}



#endif
