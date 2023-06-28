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
    const char *fn,
    FILE *fp)
{
  const int version = 3;
  int itmp;

  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.text.v3: passing null pointer to at_mb__read_text_low_level\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* clear data before reading */
  at_mb__clear(mb);

  /* n: number of temperature bins */
  if (fscanf(fp, "%d", &itmp) != 1) {
    fprintf(stderr, "Error@at.mb.io.text.v3: error in reading itmp\n");
    goto ERR;
  }

  if (itmp != mb->distr->domain->n) {
    fprintf(stderr, "Error@at.mb.io.text.v3: mb->distr->domain->n mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->distr->domain->n, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  {
    double beta_min = 0.0;

    if (fscanf(fp, "%lf", &beta_min) != 1) {
      fprintf(stderr, "Error@at.mb.io.text.v3: error in reading beta_min\n");
      goto ERR;
    }
    if (fabs(beta_min - mb->distr->domain->beta_min) > 1e-6) {
      fprintf(stderr, "Error@at.mb.io.text.v3: beta_min mismatch %g (read) %g (expected)\n",
          beta_min, mb->distr->domain->beta_min);
      goto ERR;
    }
  }

  {
    double beta_max = 0.0;

    if (fscanf(fp, "%lf", &beta_max) != 1) {
      fprintf(stderr, "Error@at.mb.io.text.v3: error in reading beta_max\n");
      goto ERR;
    }
    if (fabs(beta_max - mb->distr->domain->beta_max) > 1e-6) {
      fprintf(stderr, "Error@at.mb.io.text.v3: beta_max mismatch %g (read) %g (expected)\n",
          beta_max, mb->distr->domain->beta_max);
      goto ERR;
    }
  }

  /* m: maximal number of bins in a window */
  if (fscanf(fp, "%d", &itmp) != 1) {
    fprintf(stderr, "Error@at.mb.io.text.v3: error in reading itmp\n");
    goto ERR;
  }

  if (itmp != mb->win->max_win_bins) {
    fprintf(stderr, "Error@at.mb.io.text.v3: mb->m mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->win->max_win_bins, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* total_visits: total number of visits, number of tempering steps */
  {
    double total_visits = 0.0;

    if (fscanf(fp, "%lf", &total_visits) != 1) {
      fprintf(stderr, "Error@at.mb.io.text.v3: error in reading mb->total_visits\n");
      goto ERR;
    }
    if (total_visits <= 0) {
      fprintf(stderr, "Error@at.mb.io.text.v3: mb->total_visits: failed validation: mb->total_visits > 0\n");
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    fprintf(stderr, "Info@at.mb.io: setting mb->total_visits %g => %g\n",
      mb->total_visits, total_visits);
    mb->total_visits = total_visits;
  }

  {
    double visits = 0.0;
    int i, n = mb->distr->domain->n;

    for (i = 0; i < n; i++) {
      if (fscanf(fp, "%lf", &visits) != 1) {
        fprintf(stderr, "Error@at.mb.io.text.v3: error in reading mb->visits[%d]\n", i);
        goto ERR;
      }
      if (visits <= 0) {
        fprintf(stderr, "Error@at.mb.io.text.v3: mb->visits[%d]: failed validation: mb->visits > 0\n", i);
        goto ERR;
      }

      fprintf(stderr, "Info@at.mb.io: setting mb->visits[%d] %g => %g\n",
          i, mb->visits[i], visits);
      mb->visits[i] = visits;
    }
  }

  if (at_mb_shk__read_text(mb->shk, fn, fp, version) != 0) {
    goto ERR;
  }

  if (at_mb_accum__read_text(mb->accum, fn, fp, version) != 0) {
    goto ERR;
  }

  return 0;

ERR:
  at_mb__clear(mb);
  return -1;
}



int at_mb__write_text_v3_low_level(
    at_mb_t *mb,
    const char *fn,
    FILE *fp)
{
  const int version = 3;

  if (mb == NULL) {
    fprintf(stderr, "Error@at.mb.io.text.v3: passing null pointer to at_mb__write_text_low_level\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  fprintf(fp, "%d %g %g %d %.15e\n",
      mb->distr->domain->n,
      mb->distr->domain->beta_min,
      mb->distr->domain->beta_max,
      mb->win->max_win_bins,
      mb->total_visits);

  {
    int i, n = mb->distr->domain->n;

    for (i = 0; i < n; i++) {
      fprintf(fp, "%.15e\n", mb->visits[i]);
    }
  }

  if (at_mb_shk__write_text(mb->shk, fn, fp, version) != 0) {
    goto ERR;
  }

  if (at_mb_accum__write_text(mb->accum, fn, fp, version) != 0) {
    goto ERR;
  }

  return 0;

ERR:
  return -1;
}



#endif
