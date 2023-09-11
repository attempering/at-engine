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

#include "../distr/at_distr.h"



int at_mb__write_ze_file(at_mb_t *mb, const char *fname)
{
  int i, ip, n = mb->distr->domain->n;
  FILE *fp;

  if (fname == NULL) {
    fname = mb->ze_file;
  }

  zcom_utils__exit_if (fname == NULL,
      "Error@at.mb: file name is NULL");

  at_mb_accum__calc_win_total(mb->accum);

  at_mb_iie_gridvals__calc(mb);

  if ((fp = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "Error@at.mb: failed to open %s.\n", fname);
    return 1;
  }

  for (i = 0; i <= n; i++) {
    fprintf(fp, "%16.10f %20.10f %22.10f %22.10f ",
      mb->distr->domain->barr[i], // 1
      mb->iie->gridvals->items[i].lnz, // 2
      mb->iie->gridvals->items[i].e, // 3
      mb->iie->gridvals->items[i].cv); // 4

    ip = (i < n) ? i : (i-1); /* for quantities with no [n] */

    fprintf(fp, " %22.10f %s %+10.6f %22.10e %22.10e %22.10e %22.10e %g",
        mb->iie->et->items[ip].value, // 5
        at_mb_iie_gridvals_item__quality_bits_to_string(&mb->iie->gridvals->items[i]), // 6 
        0.0, // mb->iie->imbal[ip], // 7
        mb->accum->sums[ip].s, // 8
        mb->visits[ip], // 9
        mb->shk->win_mul[ip], // 10
        mb->accum->win_total[ip], // 11
        mb->distr->weights->ens_w[ip]);

    fprintf(fp, "\n");
  }

  fclose(fp);

  return 0;
}


void at_mb__add(at_mb_t *mb, double e, double beta,
    int *pib, double *pinvwf, double *neg_dlnwf_dbeta)
{
  double invwf, f = 1.0, neg_dlnf_dbeta = 0.0;

  int ib = at_distr__beta_to_index(mb->distr, beta, AT__TRUE);

  *pib = ib;

  mb->visits[ib] += 1.0;
  mb->total_visits += 1.0;

  /* calculate the inverse ensemble weight
   *
   * f: f(beta);
   * neg_dlnf_dbeta: -dlnf/dbeta;
   * invwf: 1/w(beta)/f(beta);
   * neg_dlnwf_dbeta: -dln(w(beta)f(beta))/dbeta;
   * ginvwf: adaptive weight = ampf * invwf;
   */
  invwf = at_distr_weights__calc_inv_weight_bounded(
      mb->distr->weights,
      beta, neg_dlnwf_dbeta,
      &f, &neg_dlnf_dbeta);

  if (pinvwf != NULL) {
    *pinvwf = invwf;
  }

  at_mb_accum__add(mb->accum, ib, invwf, e, mb->need_cv, mb->shk, mb->total_visits);

}



void at_mb__refresh_et(at_mb_t *mb)
{
  int i;

  for (i = 0; i < mb->distr->domain->n; i++) {
    at_mb_iie_et__calc_et(mb->iie, i, NULL);
  }
}



#endif
