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

#ifndef AT_MB_UTILS__SRC_H__
#define AT_MB_UTILS__SRC_H__

#include "at_mb_basic.h"
#include "iie/at_mb_iie.h"
#include "accum/at_mb_accum.h"
#include "at_mb_utils.h"

#include "../../zcom/zcom.h"




int at_mb__write_ze_file(at_mb_t *mb, const char *fname)
{
  int i, ip;
  FILE *fp;

  if (fname == NULL) {
    fname = mb->ze_file;
  }

  zcom_util__exit_if (fname == NULL, "file name is NULL");

  at_mb_accum__calc_win_total(mb->accum);

  at_mb_iie_gridvals__calc(mb);

  if ((fp = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "cannot open %s.\n", fname);
    return 1;
  }

  for (i = 0; i <= mb->n; i++) {
    fprintf(fp, "%16.10f %20.10f %22.10f %22.10f ",
      mb->barr[i], // 1
      mb->iie->gridvals->items[i].lnz, // 2
      mb->iie->gridvals->items[i].e, // 3
      mb->iie->gridvals->items[i].cv); // 4

    ip = (i < mb->n) ? i : (i-1); /* for quantities with no [mb->n] */

    fprintf(fp, " %22.10f %s %+10.6f %22.10e %22.10e %22.10e %22.10e",
      mb->iie->et->items[ip].value, // 5
      at_mb_iie_gridvals_item__quality_bits_to_string(&mb->iie->gridvals->items[i]), // 6 
      0.0, // mb->iie->imbal[ip], // 7
      mb->accum->sums[ip].s, // 8
      mb->visits[ip], // 9
      mb->shk->win_multiplier[ip], // 10
      mb->accum->win_total[ip]);

    fprintf(fp, "\n");
  }

  fclose(fp);

  return 0;
}


int at_mb__beta_to_index(at_mb_t *mb, double beta, int check)
{
  int j;

  if (beta > mb->bmin) {
    j = (int)((beta - mb->bmin)/mb->bdel);
  } else {
    j = -1;
  }

  if (check) {
    zcom_util__exit_if (j < 0 || j >= mb->n, "beta = %d, %g, range = (%g, %g, %g)\n",
        j, beta, mb->bmin, mb->bdel, mb->bmax);
  }

  return j;
}


void at_mb__add(at_mb_t *mb, double e, double beta,
    int *pib, double *pinvwf, double *neg_dlnwf_dbeta)
{
  double invwf, f = 1.0, neg_df_dbeta = 0.0;
  int bCv = mb->flags & MB_CV;

  int j = at_mb__beta_to_index(mb, beta, 1);

  *pib = j;

  mb->visits[j] += 1.0;
  mb->total_visits += 1.0;

  /* calculate the inverse ensemble weight
   *
   * f: f(beta);
   * neg_df_dbeta: -df/dbeta;
   * invwf: 1/w(beta)/f(beta);
   * neg_dlnwf_dbeta: -dln(w(beta)f(beta))/dbeta;
   * ginvwf: adaptive weight = ampf * invwf;
   */
  invwf = at_mb_betadist__calc_inv_weight(mb->betadist, beta, neg_dlnwf_dbeta, &f, &neg_df_dbeta);

  if (pinvwf != NULL) {
    *pinvwf = invwf;
  }

  at_mb_accum__add(mb->accum, j, invwf, e, bCv, mb->shk, mb->total_visits);

}



void at_mb__refresh_et(at_mb_t *mb, int reps)
{
  int i, rep;

  for (rep = 0; rep < reps; rep++) {
    for (i = 0; i < mb->n; i++)
      at_mb_iie_et__calc_et(mb->iie, i);
  }
}



#endif
