/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

#ifndef AT_MB_BETADIST__DEF_H__
#define AT_MB_BETADIST__DEF_H__

/* parameters for the overall beta disribution */
typedef struct at_mb_betadist_t_ {
  int       mode;         /* mode 0: disable 1: Gaussian 2:exponential */
  double    ens_exp;      /* ensemble exponent of beta */
  double    beta0;        /* beta0 in Gaussian distribution */
  double    invsigma2;    /* 1/sigma^2 in Gaussian distribution */
  double    c;            /* c in exponential distribution */
  double   *ens_w;        /* array of ensemble weights at bin boundaries */
  int       n;            /* number of beta bins == mb->n */
  double    beta_min;     /* minimal beta == mb->bmin */
  double    beta_max;     /* maximal beta == mb->bmax */
} at_mb_betadist_t;

#endif
