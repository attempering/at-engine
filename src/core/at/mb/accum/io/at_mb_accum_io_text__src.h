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

#ifndef AT_MB_ACCUM_IO_TEXT__SRC_H__
#define AT_MB_ACCUM_IO_TEXT__SRC_H__


#include "at_mb_accum_io_text.h"
#include "../at_mb_accum.h"
#include "../../sm/at_mb_sm.h"



int at_mb_accum__read_text(
    at_mb_accum_t *accum,
    at_utils_io_t *io)
{
  int i;

  zcom_utils__exit_if (accum->sums == NULL,
      "Error@at.mb.accum: accum->sums is null\n");

  if (at_utils_io_text__read_token(io, "ACCUM_BEGIN") != 0) {
    goto ERR;
  }

  for (i = 0; i < accum->n; i++) {

    if (at_utils_io_text__read_and_compare_int(io, NULL, "at.mb.accum:i", i) != 0) {
      goto ERR;
    }

    if (0 != at_mb_sm__read_text(accum->sums+i, io->fp)) {
      fprintf(stderr, "Error@at.mb.accum.io: error reading accum->sums+%d\n", i);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

  }

  if (at_mb_accum_winaccum__read_text(accum->winaccum, io) != 0) {
    goto ERR;
  }

  if (at_utils_io_text__read_token(io, "ACCUM_END") != 0) {
    goto ERR;
  }

  /* recalculate accum->win_total */
  at_mb_accum__calc_win_total(accum);

  return 0;

ERR:

  return -1;
}



int at_mb_accum__write_text(
    at_mb_accum_t *accum,
    at_utils_io_t *io)
{
  int i;
  at_mb_sm_t *sm;

  /* sums: normal data */
  zcom_utils__exit_if (accum->sums == NULL,
      "Error@at.mb.accum: accum->sums is null\n");

  at_utils_io_text__write_token(io, "ACCUM_BEGIN");

  for (i = 0; i < accum->n; i++) {

    if (at_utils_io_text__write_int(io, i, "at.mb.accum:i") != 0) {
      goto ERR;
    }

    sm = at_mb_accum__get_sums(accum, i);

    if (0 != at_mb_sm__write_text(sm, io->fp)) {
      fprintf(stderr, "Error@at.mb.accum.io: error writing accum->sums+%d\n", i);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  if (at_mb_accum_winaccum__write_text(accum->winaccum, io) != 0) {
    goto ERR;
  }

  at_utils_io_text__write_token(io, "ACCUM_END");

  return 0;

ERR:

  return -1;
}



#endif
