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



int at_mb_accum_winaccum__read_binary(at_mb_accum_winaccum_t *winaccum,
    const char *fn, FILE *fp, int version, int endn)
{

  int i;
  at_bool_t winaccum_enabled = winaccum->enabled;

  if (version >= 3) {
    int itmp;

    if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: failed to read winaccum->enabled\n");
      goto ERR;
    }

    winaccum_enabled = itmp;

    if (winaccum_enabled != winaccum->enabled) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: winaccum->enabled %d, file %d\n",
          winaccum->enabled, winaccum_enabled);
      goto ERR;
    }
  }

  if (winaccum_enabled) {

    for (i = 0; i < winaccum->n; i++) {

      int ret = at_mb_accum_winaccum_item__read_binary(winaccum->items+i,
          fn, fp, version, endn);

      if (ret != 0) {
        goto ERR;
      }

    }

  }

  return 0;

ERR:

  return -1;
}



int at_mb_accum_winaccum__write_binary(at_mb_accum_winaccum_t *winaccum,
    const char *fn, FILE *fp, int version)
{
  int i, ret;

  if (version >= 3) {
    i = (winaccum->enabled ? 1 : 0);
    if (zcom_endn__fwrite(&i, sizeof(int), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: failed to write winaccum->enabled\n");
      goto ERR;
    }
  }

  if (!winaccum->enabled) {
    return 0;
  }

  //fprintf(stderr, "winaccum->n %d, %s:%d\n", winaccum->n, __FILE__, __LINE__);

  for (i = 0; i < winaccum->n; i++) {

    ret = at_mb_accum_winaccum_item__write_binary(winaccum->items+i,
        fn, fp, version);

    if (ret != 0) {
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}



static int at_mb_accum_winaccum__read_text(at_mb_accum_winaccum_t *winaccum,
    const char *fn, FILE *fp, int version)
{

  int i;
  at_bool_t winaccum_enabled = winaccum->enabled;

  {
    char token[32] = "";

    if (fscanf(fp, "%32s", token) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading begin token\n");
      goto ERR;
    }

    if (strcmp(token, "WINACCUM_BEGIN") != 0) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: bad begin token [%s]\n", token);
      goto ERR;
    }
  }

  {
    int itmp;

    if (fscanf(fp, "%d", &itmp) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: failed to read winaccum->enabled\n");
      goto ERR;
    }

    winaccum_enabled = itmp;

    if (winaccum_enabled != winaccum->enabled) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: winaccum->enabled %d, file %d\n",
          winaccum->enabled, winaccum_enabled);
      goto ERR;
    }
  }

  if (winaccum_enabled) {

    for (i = 0; i < winaccum->n; i++) {

      int ret = at_mb_accum_winaccum_item__read_text(winaccum->items+i,
          fn, fp, version);

      if (ret != 0) {
        goto ERR;
      }

    }

  }

  {
    char token[32] = "";

    if (fscanf(fp, "%32s", token) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading end token\n");
      goto ERR;
    }

    if (strcmp(token, "WINACCUM_END") != 0) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: bad end token [%s]\n", token);
      goto ERR;
    }
  }

  return 0;

ERR:

  return -1;
}



static int at_mb_accum_winaccum__write_text(at_mb_accum_winaccum_t *winaccum,
    const char *fn, FILE *fp, int version)
{
  int i, ret;

  fprintf(fp, "WINACCUM_BEGIN\n");

  {
    i = (winaccum->enabled ? 1 : 0);
    fprintf(fp, "%d\n", i);
  }

  if (!winaccum->enabled) {
    return 0;
  }

  //fprintf(stderr, "winaccum->n %d, %s:%d\n", winaccum->n, __FILE__, __LINE__);

  for (i = 0; i < winaccum->n; i++) {

    ret = at_mb_accum_winaccum_item__write_text(winaccum->items+i,
        fn, fp, version);

    if (ret != 0) {
      goto ERR;
    }

  }

  fprintf(fp, "WINACCUM_END\n");

  return 0;

ERR:

  return -1;
}



#endif
