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

#ifndef AT_MB_ACCUM_WINACCUM_IO_BINARY__SRC_H__
#define AT_MB_ACCUM_WINACCUM_IO_BINARY__SRC_H__


#include "../at_mb_accum_winaccum.h"

#include "../../../sm/at_mb_sm.h"
#include "../../../../../zcom/zcom.h"


int at_mb_accum_winaccum__read_binary(
    at_mb_accum_winaccum_t *winaccum,
    at_utils_io_t *io)
{
  at_bool_t winaccum_enabled = winaccum->enabled;

  if (io->version >= 3) {

    if (at_utils_io_binary__read_and_compare_bool(io, &winaccum_enabled, "winaccum_enabled", winaccum->enabled) != 0) {
      goto ERR;
    }

  }

  if (winaccum_enabled) {

    int i;

    for (i = 0; i < winaccum->n; i++) {

      if (at_mb_accum_winaccum_item__read_binary(winaccum->items+i, io) != 0) {
        goto ERR;
      }

    }

  }

  return 0;

ERR:

      printf("err: %d %d\n", io->version, winaccum->enabled);getchar();


  return -1;
}



int at_mb_accum_winaccum__write_binary(
    at_mb_accum_winaccum_t *winaccum,
    at_utils_io_t *io)
{
  int i;

  if (io->version >= 3) {

    if (at_utils_io_binary__write_bool(io, winaccum->enabled, "winaccum_enabled") != 0) {
      goto ERR;
    }

  }

  if (!winaccum->enabled) {
    return 0;
  }

  //fprintf(stderr, "winaccum->n %d, %s:%d\n", winaccum->n, __FILE__, __LINE__);

  for (i = 0; i < winaccum->n; i++) {

    if (at_mb_accum_winaccum_item__write_binary(winaccum->items+i, io) != 0) {
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}


#endif
