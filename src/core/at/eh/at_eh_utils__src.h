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

#ifndef AT_EH_UTILS__SRC_H__
#define AT_EH_UTILS__SRC_H__

#include "at_eh_utils.h"

#include "../distr/at_distr.h"
#include <stdio.h>
#include <math.h>


void at_eh__add(at_eh_t *eh, int ib, double e)
{
  if (eh->mode > 0) { /* add to energy histogram */

    int ie = (int)((e - eh->min)/eh->del);
    if (ie >= 0 && ie < eh->cnt) /* no invw for histogram */
      eh->his[ib*eh->cnt+ie] += 1.0;
  }
}


int at_eh__reconstruct(at_eh_t *eh, const char *fname)
{
  FILE *fp;
  int ib, j, js, jt, ie, imin, imax, cols, full, keep0;
  double eav, db, x;
  double num, den;
  double del, base, inc;
  at_mb_t *mb = eh->mb;
  at_mb_iie_t *iie = mb->iie;
  at_distr_t *distr = mb->distr;
  int n = distr->domain->n;

  if (eh == NULL || eh->mode == 0) {
    return 0;
  }
  
  if (eh->mode != 1) {
    fprintf(stderr, "invalid eh_mode %d\n", eh->mode);
    return -1;
  }

  if ((fp = fopen((fname != NULL) ? fname : eh->rfile, "w")) == NULL) {
    fprintf(stderr, "cannot write reconstructed histogram [%s].\n",
        eh->rfile);
    return 1;
  }
  full = eh->keep_margins;
  keep0 = !eh->no_zeros;
  del = (eh->add_half_ebin) ? 0.5 : 0; /* for continuous system */
  cols = eh->cnt;
  base = eh->min;
  inc = eh->del;
  db = distr->domain->bdel;

  /* build lnZ */
  for (iie->gridvals->items[0].lnz = 0.0, ib = 0; ib < n; ib++) {
    double et = iie->et->items[ib].value;
    iie->gridvals->items[ib+1].lnz = iie->gridvals->items[ib].lnz
      + et*(distr->domain->barr[ib] - distr->domain->barr[ib+1]);
  }

  /* loop over temperatures, and skip a few intermediate temperatures */
  for (ib = 0; ib <= n; ib += eh->skip) {
    /* reconstruct energy histogram at beta = distr->domain->barr[ib] */
    js = eh->is[ib];
    jt = eh->it[ib];
    if (js < 0 || jt > n || js >= jt) {
      fprintf(stderr, "bad window (%d, %d)\n", js, jt);
      return -1;
    }

    /* loop over energy bins */
    for (ie = 0; ie < cols; ie++) {
      eav = base + (ie+del)*inc;
      for (den = 0, j = js; j <= jt; j++) { /* denominator */
        double exp_fac = -eav*db*(j - ib)
                       - iie->gridvals->items[j].lnz
                       + iie->gridvals->items[ib].lnz;
        x = distr->weights->ens_w[j] * exp(exp_fac);
        if (j == js || j == jt) {
          x *= 0.5;
        }
        den += x;
      }
      for (num = 0.0, j = js; j < jt; j++) { /* numerator */
        x = eh->his[j*cols + ie];
        if (fabs(x) < 0.5) continue;
        num += x;
      }
      eh->recon[ie] = num/den;
    }

    /* determine the output range */
    if (full) {
      imin = 0;
      imax = cols;
    } else { /* only print the smallest non-zero region */
      for (ie = cols-1; ie >= 0; ie--) if (eh->recon[ie] > 0) break;
      if ((imax=ie+1) == 0) continue;
      for (imin = 0; imin < imax; imin++) if (eh->recon[imin] > 0) break;
    }
    /* normalize the histogram, and output */
    for (x = 0, ie = imin; ie < imax; ie++) {
      x += eh->recon[ie];
    }

    x = (fabs(x) > AT_MB_ACCUM_MIN_SIZE) ? 1.0/(x*inc) : 1.0;

    for (ie = imin; ie < imax; ie++) {
      if (keep0 || eh->recon[ie] > AT_MB_ACCUM_MIN_SIZE) {
        fprintf(fp, "%g %.14E %g\n",
            base + (ie+del)*inc, eh->recon[ie]*x,
            distr->domain->barr[ib]);
      }
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
  return 0;
}

#endif
