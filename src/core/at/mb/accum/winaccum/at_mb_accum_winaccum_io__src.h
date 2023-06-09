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

#ifndef AT_MB_ACCUM_WINACCUM_IO__SRC_H__
#define AT_MB_ACCUM_WINACCUM_IO__SRC_H__


#include "at_mb_accum_winaccum.h"

#include "../../sm/at_mb_sm.h"
#include "../../../../zcom/zcom.h"



void at_mb_accum_winaccum__manifest(const at_mb_accum_winaccum_t *winaccum,
    at_utils_manifest_t *manifest)
{

  /* use adaptive averaging */
  at_utils_manifest__print_bool(manifest, winaccum->enabled, "mb->accum->winaccum->enabled", "mb-use-adaptive-averaging");

}



int at_mb_accum_winaccum__read_binary(at_mb_accum_winaccum_t *winaccum, FILE *fp, int endn)
{

  int i;

  zcom_util__exit_if(!winaccum->enabled, "winaccum is disabled\n");

  for (i = 0; i < winaccum->n; i++) {

    if (at_mb_accum_winaccum_item__read_binary(winaccum->items+i, fp, endn) != 0) {
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}



int at_mb_accum_winaccum__write_binary(at_mb_accum_winaccum_t *winaccum, FILE *fp)
{
  int i;

  if (!winaccum->enabled) return 0;

  //fprintf(stderr, "winaccum->n %d, %s:%d\n", winaccum->n, __FILE__, __LINE__);

  for (i = 0; i < winaccum->n; i++) {

    if (at_mb_accum_winaccum_item__write_binary(winaccum->items+i, fp) != 0) {
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}



#endif
