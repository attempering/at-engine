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
#ifndef AT_MB_IO_LEGACY_BINARY__SRC_H__
#define AT_MB_IO_LEGACY_BINARY__SRC_H__

#include "at_mb_io_legacy_binary.h"

/* implementation headers */
#include "../at_mb_basic.h"
#include "../sm/at_mb_sm.h"
#include "../accum/at_mb_accum.h"
#include "../../distr/at_distr.h"


static int at_mb__read_binary_legacy_low_level(
    at_mb_t *mb,
    at_driver_langevin_t *langevin,
    double *beta,
    FILE *fp, int version, int endn)
{
  int itmp;
  at_distr_domain_t *domain = mb->distr->domain;

  (void) version;

  if (mb == NULL) {
    fprintf(stderr, "passing null pointer to at_mb__read_binary_low_level\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* clear data before reading */
  at_mb__clear(mb);
  at_driver_langevin__clear(langevin);

  /* n: number of temperature bins */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->distr->domain->n) {
    fprintf(stderr, "mb->distr->domain->n mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->distr->domain->n, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* m: maximal number of bins in a window */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->win->max_win_bins) {
    fprintf(stderr, "mb->m mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->win->max_win_bins, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  {
    int mb_order = 1;

    /* order: order, should be 1 */
    if (zcom_endn__fread(&mb_order, sizeof(mb_order), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading mb_order\n");
      goto ERR;
    }
    if ( !(mb_order == 1) ) {
      fprintf(stderr, "mb->order: failed validation: mb_order == 1\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* flags: combination of flags */
  {
    unsigned flags;

    if (zcom_endn__fread(&flags, sizeof(flags), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading flags\n");
      goto ERR;
    }
  }

  /* use_winaccum */
  if (zcom_endn__fread(&mb->accum->winaccum->enabled, sizeof(mb->accum->winaccum->enabled), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->accum->winaccum->enabled\n");
    goto ERR;
  }

  /* cnt_int: number of additional integer variables to be written to binary file */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->cnt_int) {
    fprintf(stderr, "mb->cnt_int mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->cnt_int, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* cnt_dbl: number of additional double variables to be written to binary file */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->cnt_dbl) {
    fprintf(stderr, "mb->cnt_dbl mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->cnt_dbl, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  {
    /* beta: current value of beta */
    if (zcom_endn__fread(beta, sizeof(*beta), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading beta\n");
      goto ERR;
    }

    if ( !(*beta >= domain->beta_min && *beta <= domain->beta_max) ) {
      fprintf(stderr, "beta: failed validation: beta >= beta_min && beta <= beta_max\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    if (*beta > domain->beta_max - 1e-5) {
      *beta = domain->beta_max - 1e-5;
    } else if (*beta < domain->beta_min + 1e-5) {
      *beta = domain->beta_min + 1e-5;
    }

  }


  /* total_visits: total number of visits, number of tempering */
  if (zcom_endn__fread(&mb->total_visits, sizeof(mb->total_visits), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->total_visits\n");
    goto ERR;
  }
  if ( !(mb->total_visits > 0) ) {
    fprintf(stderr, "mb->total_visits: failed validation: mb->total_visits > 0\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* shk_base: current generic shrink amplitude */
  if (zcom_endn__fread(&mb->shk->base, sizeof(mb->shk->base), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->shk_base\n");
    goto ERR;
  }

  if (at_driver_langevin__read_binary_legacy(langevin, fp, endn) != 0) {
    goto ERR;
  }

  /* barr: temperature array */
  if ((mb->distr->domain->n > 0)
    && (zcom_endn__fread(mb->distr->domain->barr, sizeof(*(mb->distr->domain->barr)), mb->distr->domain->n, fp, endn) != (size_t) (mb->distr->domain->n))) {
    fprintf(stderr, "error in reading mb->distr->domain->barr, n = mb->distr->domain->n(%d)\n", mb->distr->domain->n);
    goto ERR;
  }
  /* et: bin-averaged internal energy */
  /*
  if ((mb->distr->domain->n > 0)
    && (zcom_endn__fread(mb->iie->et, sizeof(*(mb->iie->et)), mb->distr->domain->n, fp, endn) != (size_t) (mb->distr->domain->n))) {
    fprintf(stderr, "error in reading mb->et, n = mb->distr->domain->n(%d)\n", mb->distr->domain->n);
    goto ERR;
  }
  */

  if (at_mb_accum__read_binary(mb->accum, fp, endn) != 0) {
    goto ERR;
  }

  return 0;

ERR:
  at_mb__clear(mb);
  at_driver_langevin__clear(langevin);
  return -1;
}



int at_mb__read_binary_legacy(
    at_mb_t *mb,
    at_driver_langevin_t *langevin, 
    double *beta,
    const char *fname,
    int *pver)
{
  FILE *fp;
  int ver;
  int itmp;
  int i;
  int endn;

  if ((fp = fopen(fname, "rb")) == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: failed to read binary file [%s].\n", fname);
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* determine file endian */
  if ((endn = zcom_endn__rmatchi(&itmp, sizeof(int), fp)) < 0) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: itmp 0x%X failed to match sizeof(int) 0x%X\n",
        (unsigned) itmp, (unsigned) sizeof(int));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != (int) sizeof(double)) {
    fprintf(stderr, "(int) sizeof(double) mismatch, expect: %d, read: %d, pos: %#lx\n",
        (int) sizeof(double), itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if (zcom_endn__fread(&ver, sizeof(ver), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading ver\n");
    goto ERR;
  }
  if (pver != NULL) {
    *pver = ver;
  }

  /* call low level read function for members */
  i = at_mb__read_binary_legacy_low_level(mb, langevin, beta, fp, ver, endn);

  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}



static int at_mb__write_binary_legacy_low_level(
    at_mb_t *mb,
    at_driver_langevin_t *langevin,
    double beta,
    FILE *fp,
    int version)
{
  (void) version;

  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: passing null pointer to at_mb__write_binary_low_level\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }
  /* n: number of temperature bins */
  if (zcom_endn__fwrite(&mb->distr->domain->n, sizeof(mb->distr->domain->n), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->distr->domain->n\n");
    goto ERR;
  }
  /* m: maximal number of bins in a window */
  if (zcom_endn__fwrite(&mb->win->max_win_bins, sizeof(mb->win->max_win_bins), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->m\n");
    goto ERR;
  }

  {
    int mb_order = 1;

    if (zcom_endn__fwrite(&mb_order, sizeof(mb_order), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb_order\n");
      goto ERR;
    }
  }

  {
    unsigned flags = 0;

    if (mb->accum->winaccum->enabled) {
      flags |= AT_MB__USE_WIN_ACCUM;
    }

    if (mb->need_cv) {
      flags |= AT_MB__NEED_CV;
    }

    if (mb->use_sym_wins) {
      flags |= AT_MB__USE_SYM_WINS;
    }

    if (mb->use_single_bin) {
      flags |= AT_MB__USE_SINGLE_BIN;
    }

    if (mb->verbose) {
      flags |= AT_MB__VERBOSE;
    }

    /* flags: combination of flags */
    if (zcom_endn__fwrite(&flags, sizeof(flags), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing flags\n");
      goto ERR;
    }
  }

  /* use_winaccum */
  if (zcom_endn__fwrite(&mb->accum->winaccum->enabled, sizeof(mb->accum->winaccum->enabled), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->accum->winaccum->enabled\n");
    goto ERR;
  }

  /* cnt_int: number of additional integer variables to be written to binary file */
  if (zcom_endn__fwrite(&mb->cnt_int, sizeof(mb->cnt_int), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->cnt_int\n");
    goto ERR;
  }
  /* cnt_dbl: number of additional double variables to be written to binary file */
  if (zcom_endn__fwrite(&mb->cnt_dbl, sizeof(mb->cnt_dbl), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->cnt_dbl\n");
    goto ERR;
  }

  {
    /* beta: current value of beta */
    if (zcom_endn__fwrite(&beta, sizeof(beta), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->beta\n");
      goto ERR;
    }
  }

  /* total_visits: total number of visits, number of tempering */
  if (zcom_endn__fwrite(&mb->total_visits, sizeof(mb->total_visits), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->total_visits\n");
    goto ERR;
  }
  /* shk_base: current generic shrink amplitude */
  if (zcom_endn__fwrite(&mb->shk->base, sizeof(mb->shk->base), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->shk->base\n");
    goto ERR;
  }

  if (at_driver_langevin__write_binary_legacy(langevin, fp) != 0) {
    goto ERR;
  }

  /* barr: temperature array */
  if ((mb->distr->domain->n > 0)
    && (zcom_endn__fwrite(mb->distr->domain->barr, sizeof(*(mb->distr->domain->barr)), mb->distr->domain->n, fp, 1) != (size_t) (mb->distr->domain->n))) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing mb->distr->domain->barr, n = mb->distr->domain->n(%d)\n", mb->distr->domain->n);
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

  if (at_mb_accum__write_binary(mb->accum, fp) != 0) {
    goto ERR;
  }

  return 0;
ERR:
  return -1;
}



int at_mb__write_binary_legacy(
    at_mb_t *mb,
    at_driver_langevin_t *langevin,
    double beta,
    const char *fname,
    int ver)
{
  FILE *fp;
  int i;
  int size;

  if ((fp = fopen(fname, "wb")) == NULL) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: failed to write binary file [%s].\n", fname);
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  size = (int) sizeof(int);
  if (zcom_endn__fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing size\n");
    goto ERR;
  }
  size = (int) sizeof(double);
  if (zcom_endn__fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing size\n");
    goto ERR;
  }
  if (zcom_endn__fwrite(&ver, sizeof(ver), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.io.binary.legacy: error in writing ver\n");
    goto ERR;
  }

  /* call low level write function for members */
  i = at_mb__write_binary_legacy_low_level(mb, langevin, beta, fp, ver);

  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}


#endif
