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
    const char *fn,
    FILE *fp,
    int endn)
{
  const int version = 2;
  int itmp;
  at_utils_io_t io[1];

  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: passing null pointer to at_mb__read_binary_low_level\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* clear data before reading */
  at_mb__clear(mb);

  at_utils_io_binary__init_read(io, "at.mb.io.binary.v2", fn, fp, endn);

  /* n: number of temperature bins */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in reading itmp\n");
    goto ERR;
  }

  if (itmp != mb->distr->domain->n) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: mb->distr->domain->n mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->distr->domain->n, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* m: maximal number of bins in a window */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in reading itmp\n");
    goto ERR;
  }

  if (itmp != mb->win->max_win_bins) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: mb->m mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->win->max_win_bins, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* total_visits: total number of visits, number of tempering */
  if (zcom_endn__fread(&mb->total_visits, sizeof(mb->total_visits), 1, fp, endn) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in reading mb->total_visits\n");
    goto ERR;
  }
  if (mb->total_visits <= 0) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: mb->total_visits: failed validation: mb->total_visits > 0\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* shk_base: current generic shrink amplitude */
  if (zcom_endn__fread(&mb->shk->base, sizeof(mb->shk->base), 1, fp, endn) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in reading mb->shk_base\n");
    goto ERR;
  }

  /* barr: temperature array */
  if ((mb->distr->domain->n > 0)
    && (zcom_endn__fread(mb->distr->domain->barr, sizeof(*(mb->distr->domain->barr)), mb->distr->domain->n, fp, endn) != (size_t) (mb->distr->domain->n))) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in barr, n(%d)\n", mb->distr->domain->n);
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



int at_mb__write_binary_v2_low_level(
    at_mb_t *mb,
    const char *fn,
    FILE *fp)
{
  const int version = 2;
  at_utils_io_t io[1];

  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: passing null pointer to at_mb__write_binary_low_level\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  at_utils_io_binary__init_write(io, "at.mb.io.binary.v2", fn, fp);

  /* n: number of temperature bins */
  if (zcom_endn__fwrite(&mb->distr->domain->n, sizeof(mb->distr->domain->n), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in writing mb->distr->domain->n\n");
    goto ERR;
  }

  /* m: maximal number of bins in a window */
  if (zcom_endn__fwrite(&mb->win->max_win_bins, sizeof(mb->win->max_win_bins), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in writing mb->win->max_win_bins\n");
    goto ERR;
  }

  /* total_visits: total number of visits, number of tempering */
  if (zcom_endn__fwrite(&mb->total_visits, sizeof(mb->total_visits), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in writing mb->total_visits\n");
    goto ERR;
  }

  /* shk_base: current generic shrink amplitude */
  if (zcom_endn__fwrite(&mb->shk->base, sizeof(mb->shk->base), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in writing mb->shk->base\n");
    goto ERR;
  }

  /* barr: temperature array */
  if ((mb->distr->domain->n > 0)
    && (zcom_endn__fwrite(mb->distr->domain->barr, sizeof(*(mb->distr->domain->barr)), mb->distr->domain->n, fp, 1) != (size_t) (mb->distr->domain->n))) {
    fprintf(stderr, "Error@at.mb.io.binary.v2: error in writing mb->distr->domain->barr, n = mb->distr->domain->n(%d)\n", mb->distr->domain->n);
    goto ERR;
  }
  /* et: bin-averaged internal energy */
  /*
  if ((mb->distr->domain->n > 0)
    && (zcom_endn__fwrite(mb->iie->et, sizeof(*(mb->iie->et)), mb->distr->domain->n, fp, 1) != (size_t) (mb->distr->domain->n))) {
    fprintf(stderr, "error in writing mb->et, n = mb->distr->domain->n(%d)\n", mb->distr->domain->n);
    goto ERR;
  }
  */

  if (at_mb_accum__write_binary(mb->accum, io) != 0) {
    goto ERR;
  }

  return 0;
ERR:
  return -1;
}



#endif
