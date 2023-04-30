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

#ifndef AT_DISTR_WEIGHTS__DEF_H__
#define AT_DISTR_WEIGHTS__DEF_H__

#include "../../context/at_context__def.h"

typedef struct at_distr_weights_t_ {

  double    beta_min;
  double    beta_max;
  int       n;

  int       mode;         /* mode 0: disable 1: Gaussian 2:exponential */
  double    ens_exp;      /* ensemble exponent of beta */
  double    beta0;        /* beta0 in Gaussian distribution */
  double    inv_sigma2;   /* 1/sigma^2 in Gaussian distribution */
  double    c;            /* c in exponential distribution */
  double   *ens_w;        /* array of ensemble weights at bin boundaries */

} at_distr_weights_t;

#endif
