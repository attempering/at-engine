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

#ifndef AT_MB_ACCUM_WINACCUM_ITEM_IO_TEXT__SRC_H__
#define AT_MB_ACCUM_WINACCUM_ITEM_IO_TEXT__SRC_H__


#include "at_mb_accum_winaccum_item_io_text.h"
#include "../at_mb_accum_winaccum_item_utils.h"

#include "../../../../../utils/at_utils.h"
#include "../../../../../../zcom/zcom.h"



int at_mb_accum_winaccum_item__read_text(
    at_mb_accum_winaccum_item_t *item,
    at_utils_io_t *io)
{
  int j;
  at_mb_sm_t *sm;

  /* item index */
  if (at_utils_io_text__read_and_compare_int(io, NULL, "winaccum_item->i", item->i) != 0) {
    goto ERR;
  }

  if (io->version >= 3) {

    if (at_utils_io_text__read_and_compare_int(io, NULL, "winaccum_item->js", item->js) != 0) {
      goto ERR;
    }

    if (at_utils_io_text__read_and_compare_int(io, NULL, "winaccum_item->jt", item->jt) != 0) {
      goto ERR;
    }

    if (at_utils_io_text__read_double(io, &item->amplifier, "winaccum_item->amplifier", 0) != 0) {
      goto ERR;
    }

    if (at_utils_io_text__read_double(io, &item->visits, "winaccum_item->visits", 0) != 0) {
      goto ERR;
    }

  }

  for (j = item->js; j < item->jt; j++) {

    /* read the bin index */
    if (at_utils_io_text__read_and_compare_int(io, NULL, "winaccum:bin-index", j) != 0) {
      goto ERR;
    }

    /* read the sums */
    sm = at_mb_accum_winaccum_item__get_sums(item, j);

    if (0 != at_mb_sm__read_text(sm, io->fp)) {
      fprintf(stderr, "Error@at.mb.accum.winaccum.item: [%s] error reading object winaccum->items->sums i %d j %d\n",
          io->fn, item->i, j);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}



int at_mb_accum_winaccum_item__write_text(
    at_mb_accum_winaccum_item_t *item,
    at_utils_io_t *io)
{
  int i = item->i, j;
  at_mb_sm_t *sm;

  at_mb_accum_winaccum_item__normalize(item);

  /* write the item index */
  if (at_utils_io_text__write_int(io, item->i, "winaccum_item->i") != 0) {
    goto ERR;
  }

  if (io->version >= 3) {

    if (at_utils_io_text__write_int(io, item->js, "winaccum_item->js") != 0) {
      goto ERR;
    }

    if (at_utils_io_text__write_int(io, item->jt, "winaccum_item->jt") != 0) {
      goto ERR;
    }

    if (at_utils_io_text__write_int(io, item->jt, "winaccum_item->jt") != 0) {
      goto ERR;
    }

    if (at_utils_io_text__write_double(io, item->amplifier, "winaccum_item->amplifier") != 0) {
      goto ERR;
    }

    if (at_utils_io_text__write_double(io, item->visits, "winaccum_item->visits") != 0) {
      goto ERR;
    }

  }

  for (j = item->js; j < item->jt; j++) {

    /* write the bin index */
    if (at_utils_io_text__write_int(io, j, "winaccum_item-bin-index,j") != 0) {
      goto ERR;
    }

    /* write the accumulator data */
    sm = at_mb_accum_winaccum_item__get_sums(item, j);

    if (0 != at_mb_sm__write_text(sm, io->fp)) {
      fprintf(stderr, "Error@at.mb.accum.winaccum.item: [%s] error writing object winaccum->items->sums i %d j %d\n",
          io->fn, i, j);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  return 0;
ERR:

  return -1;
}


#endif
