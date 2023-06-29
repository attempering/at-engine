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

#ifndef AT_MB_ACCUM_IO__SRC_H__
#define AT_MB_ACCUM_IO__SRC_H__


#include "at_mb_accum.h"

#include "../sm/at_mb_sm.h"

#include "../../../zcom/zcom.h"



void at_mb_accum__manifest(const at_mb_accum_t *accum, at_utils_manifest_t *manifest)
{
  int i;
  FILE *fp = manifest->fp;
  int arrmax = manifest->arr_max_items;

  /* sums: normal data */
  if (accum->sums != NULL) {
    fprintf(fp, "mb->accum->sums: at_mb_sm_t array of accum->n:");

    for (i = accum->n-1; i >= 0; i--) {
      if (accum->sums[i].s != 0) {
        break;
      }
    }

    if (i >= 0) {
      fprintf(fp, "\n");
      for (i = 0; i < accum->n; i++) {
        if (i == arrmax && i < accum->n-arrmax) {
          fprintf(fp, "\n...\n");
        }
        if (arrmax >= 0 && i >= arrmax && i < (accum->n-arrmax)) continue;
        fprintf(fp, "mb->accum->sums[%d]:\n", i);
        at_mb_sm__manifest(accum->sums+i, fp, arrmax);
      }
    } else {
      fprintf(fp, " {0}\n");
    }
  }

  at_mb_accum_winaccum__manifest(accum->winaccum, manifest);

  /* win_total: total of sum.s over a multiple-bine temperature window */
  at_utils_manifest__print_double_arr(manifest, accum->win_total, accum->n, "mb->accum->win_total");

}



int at_mb_accum__read_binary(at_mb_accum_t *accum,
    const char *fn, FILE *fp, int version, int endn)
{
  int i, itmp, ret;

  zcom_utils__exit_if (accum->sums == NULL,
      "Error@at.mb.accum: accum->sums is null\n");

  for (i = 0; i < accum->n; i++) {

    if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "Error@at.mb.accum: error in reading index %d\n", i);
      goto ERR;
    }

    if (itmp != i) {
      fprintf(stderr, "Error@at.mb.accum: i mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    if (0 != at_mb_sm__read_binary(accum->sums+i, fp, endn)) {
      fprintf(stderr, "Error@at.mb.accum: error reading object accum->sums+i %d\n", i);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

  }

  ret = at_mb_accum_winaccum__read_binary(accum->winaccum, fn, fp, version, endn);

  if (ret != 0) {
    return ret;
  }

  // recalculate accum->win_total
  at_mb_accum__calc_win_total(accum);

  return 0;

ERR:

  return -1;
}



int at_mb_accum__write_binary(at_mb_accum_t *accum,
    const char *fn, FILE *fp, int version)
{
  int i, ret;
  at_mb_sm_t *sm;

  /* sums: normal data */
  zcom_utils__exit_if (accum->sums == NULL,
      "Error@at.mb.accum: accum->sums is null\n");

  for (i = 0; i < accum->n; i++) {
    if (zcom_endn__fwrite(&i, sizeof(i), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.accum: failed to write bin index %d\n", i);
      goto ERR;
    }

    sm = at_mb_accum__get_sums(accum, i);

    if (0 != at_mb_sm__write_binary(sm, fp)) {
      fprintf(stderr, "Error@at.mb.accum: object accum->sums+i\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  ret = at_mb_accum_winaccum__write_binary(accum->winaccum,
      fn, fp, version);

  if (ret != 0) {
    return ret;
  }

  return 0;
ERR:

  return -1;
}



int at_mb_accum__read_text(at_mb_accum_t *accum,
    const char *fn, FILE *fp, int version)
{
  int i, itmp, ret;

  (void) version;

  zcom_utils__exit_if (accum->sums == NULL,
      "Error@at.mb.accum: accum->sums is null\n");

  {
    char token[32] = "";

    if (fscanf(fp, "%32s", token) != 1) {
      fprintf(stderr, "Error@at.mb.accum: error in reading begin token\n");
      goto ERR;
    }

    if (strcmp(token, "ACCUM_BEGIN") != 0) {
      fprintf(stderr, "Error@at.mb.accum: bad begin token [%s]\n", token);
      goto ERR;
    }
  }

  for (i = 0; i < accum->n; i++) {

    if (fscanf(fp, "%d", &itmp) != 1) {
      fprintf(stderr, "Error@at.mb.accum: error in reading index %d from %s\n", i, fn);
      goto ERR;
    }

    if (itmp != i) {
      fprintf(stderr, "Error@at.mb.accum: i mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    if (0 != at_mb_sm__read_text(accum->sums+i, fp)) {
      fprintf(stderr, "Error@at.mb.accum: error reading object accum->sums+i %d\n", i);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

  }

  ret = at_mb_accum_winaccum__read_text(accum->winaccum, fn, fp, version);

  if (ret != 0) {
    return ret;
  }

  // recalculate accum->win_total
  at_mb_accum__calc_win_total(accum);

  {
    char token[32] = "";

    if (fscanf(fp, "%32s", token) != 1) {
      fprintf(stderr, "Error@at.mb.accum: error in reading end token\n");
      goto ERR;
    }

    if (strcmp(token, "ACCUM_END") != 0) {
      fprintf(stderr, "Error@at.mb.accum: bad begin end [%s]\n", token);
      goto ERR;
    }
  }

  return 0;

ERR:

  return -1;
}



int at_mb_accum__write_text(at_mb_accum_t *accum,
    const char *fn, FILE *fp, int version)
{
  int i, ret;
  at_mb_sm_t *sm;

  (void) version;

  zcom_utils__exit_if (accum->sums == NULL,
      "Error@at.mb.accum: accum->sums is null\n");

  fprintf(fp, "ACCUM_BEGIN\n");

  for (i = 0; i < accum->n; i++) {

    fprintf(fp, "%d ", i);

    sm = at_mb_accum__get_sums(accum, i);

    at_mb_sm__write_text(sm, fp);
  }

  ret = at_mb_accum_winaccum__write_text(accum->winaccum,
      fn, fp, version);

  if (ret != 0) {
    return ret;
  }

  fprintf(fp, "ACCUM_END\n");

  return 0;
}



#endif
