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

/* text IO routines for at_mb_t */
#ifndef AT_MB_IO_TEXT_V3__SRC_H__
#define AT_MB_IO_TEXT_V3__SRC_H__

#include "at_mb_io_text_v3.h"

/* implementation headers */
#include "../../../at_mb_basic.h"
#include "../../../sm/at_mb_sm.h"
#include "../../../accum/at_mb_accum.h"
#include "../../../../distr/at_distr.h"

#include "../../../../../zcom/zcom.h"



int at_mb__read_text_v3_low_level(
    at_mb_t *mb,
    at_utils_io_t *io)
{
  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.v3: passing null pointer to at_mb__read_binary_low_level\n");
    return -1;
  }

  /* clear data before reading */
  at_mb__clear(mb);

  /* n: number of temperature bins */
  if (at_utils_io_text__read_and_compare_int(io, NULL, "n", mb->distr->domain->n) != 0) {
    goto ERR;
  }

  /* beta_min */
  if (at_utils_io_text__read_and_compare_double(io, NULL, "beta_min", mb->distr->domain->beta_min, 1e-8) != 0) {
    goto ERR;
  }

  /* beta_max */
  if (at_utils_io_text__read_and_compare_double(io, NULL, "beta_max", mb->distr->domain->beta_max, 1e-8) != 0) {
    goto ERR;
  }

  /* m: maximal number of bins in a window */
  if (at_utils_io_text__read_and_compare_int(io, NULL, "m", mb->win->max_win_bins) != 0) {
    goto ERR;
  }

  /* total_visits: total number of visits, number of tempering steps */
  if (at_utils_io_text__read_double(io, &mb->total_visits,
      "total_visits", AT_UTILS_IO__NONNEGATIVE) != 0) {
    goto ERR;
  }

  /* visits: number of visits to bins */
  if (at_utils_io_text__read_double_array(io, mb->distr->domain->n, mb->visits,
      "mb->visits", AT_UTILS_IO__NONNEGATIVE) != 0) {
    goto ERR;
  }

  if (at_mb_shk__read_text(mb->shk, io) != 0) {
    goto ERR;
  }

  if (at_mb_accum__read_text(mb->accum, io) != 0) {
    goto ERR;
  }

  return 0;

ERR:
  at_mb__clear(mb);
  return -1;
}



int at_mb__write_text_v3_low_level(
    at_mb_t *mb,
    at_utils_io_t *io)
{
  zcom_utils__exit_if (mb == NULL,
    "Error@at.mb.io.text.v3: passing null pointer to at_mb__write_text_low_level\n");

  fprintf(io->fp, "%d %g %g %d %.15e\n",
      mb->distr->domain->n,
      mb->distr->domain->beta_min,
      mb->distr->domain->beta_max,
      mb->win->max_win_bins,
      mb->total_visits);

  if (at_utils_io_text__write_double_array(io, mb->distr->domain->n, mb->visits, "mb->visits", 0)) {
    goto ERR;
  }

  if (at_mb_shk__write_text(mb->shk, io) != 0) {
    goto ERR;
  }

  if (at_mb_accum__write_text(mb->accum, io) != 0) {
    goto ERR;
  }

  return 0;

ERR:
  return -1;
}



#endif
