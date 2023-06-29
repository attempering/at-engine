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
#ifndef AT_MB_IO_BINARY_V2__SRC_H__
#define AT_MB_IO_BINARY_V2__SRC_H__

#include "at_mb_io_binary_v2.h"

/* implementation headers */
#include "../../../at_mb_basic.h"
#include "../../../sm/at_mb_sm.h"
#include "../../../accum/at_mb_accum.h"
#include "../../../../distr/at_distr.h"

#include "../../../../../zcom/zcom.h"



int at_mb__read_binary_v2_low_level(
    at_mb_t *mb,
    at_utils_io_t *io)
{
  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: passing null pointer to at_mb__read_binary_v2_low_level\n");
    return -1;
  }

  /* clear data before reading */
  at_mb__clear(mb);

  /* n: number of temperature bins */
  if (at_utils_io_binary__read_and_compare_int(io, NULL, "n", mb->distr->domain->n) != 0) {
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

  /* shk_base */
  if (at_utils_io_binary__read_double(io, &mb->shk->base,
      "shk_base", AT_UTILS_IO__NONNEGATIVE) != 0) {
    goto ERR;
  }

  /* barr */
  if (at_utils_io_binary__read_double_array(io, mb->distr->domain->n, NULL,
      "barr", AT_UTILS_IO__NONNEGATIVE) != 0) {
    goto ERR;
  }

  // mb->iie->et

  if (at_mb_accum__read_binary(mb->accum, io) != 0) {
    goto ERR;
  }

  return 0;

ERR:

  at_mb__clear(mb);

  return -1;
}



int at_mb__write_binary_v2_low_level(
    at_mb_t *mb,
    at_utils_io_t *io)
{
  zcom_utils__exit_if (mb == NULL,
    "Error@at.mb.io.binary.v2: passing null pointer to at_mb__write_binary_v2_low_level\n");

  if (at_utils_io_binary__write_int(io, mb->distr->domain->n, "n") != 0) {
    goto ERR;
  }

  /* m: maximal number of bins in a window */
  if (at_utils_io_binary__write_int(io, mb->win->max_win_bins, "m") != 0) {
    goto ERR;
  }

  /* total_visits: total number of visits, number of tempering steps */
  if (at_utils_io_binary__write_double(io, mb->total_visits, "mb->total_visits") != 0) {
    goto ERR;
  }

  /* shk_base */
  if (at_utils_io_binary__write_double(io, mb->shk->base, "shk_base") != 0) {
    goto ERR;
  }

  /* barr */
  if (at_utils_io_binary__write_double_array(io, mb->distr->domain->n, mb->distr->domain->barr, "barr", 0) != 0) {
    goto ERR;
  }

  // mb->iie->et

  if (at_mb_accum__write_binary(mb->accum, io) != 0) {
    goto ERR;
  }

  return 0;
ERR:
  return -1;
}



#endif
