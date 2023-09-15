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
#include "../../utils/at_utils__def.h"
#include "composite/at_distr_weights_composite__def.h"

enum {
  AT_DISTR_WEIGHTS_MODE__FLAT = 0,
  AT_DISTR_WEIGHTS_MODE__GAUSSIAN = 1,
  AT_DISTR_WEIGHTS_MODE__EXPONENTIAL = 2,
  AT_DISTR_WEIGHTS_MODE__COMPOSITE = 3,
  AT_DISTR_WEIGHTS_MODE__COUNT,
};


typedef struct at_distr_weights_t_ {

  double    beta_min;
  double    beta_max;
  int       n;

  int       mode;         /* mode: AT_DISTR_WEIGHTS_MODE__xxx */

  double    ens_exp;      /* exponent for the overall beta factor as beta^{-ens_exp}
                             This applies to every mode */
  at_bool_t ens_exp_is_int; /* whether ens_exp is an integer */
  int       ens_exp_int;  /* integer version of ens_exp */

  /* for legacy mode 1 */
  double    beta0;        /* beta0 in Gaussian distribution */
  double    sigma;        /* Gaussian width */
  double    inv_sigma2;   /* 1/sigma^2 in Gaussian distribution */

  /* for legacy mode 2 */
  double    c;            /* c in exponential distribution */

  at_distr_weights_composite_t composite[1];

  /* the ordering parameter for sorting replicas */
  double   q_order;

  double   f_min;         /* minimum value of the f factor */
  double   f_max;         /* maximum value of the f factor */

  /* the factor invwf is used to update the accumulators */
  double   invwf_min;     /* minimum value of the 1/(w*f) factor */
  double   invwf_max;     /* maximum value of the 1/(w*f) factor */

  double   *ens_w;        /* array of ensemble weights at bin boundaries */

  at_utils_logger_t logger[1]; /* module-specific logger */

} at_distr_weights_t;

#endif
