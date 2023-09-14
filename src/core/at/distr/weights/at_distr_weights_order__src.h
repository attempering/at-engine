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

#ifndef AT_DISTR_WEIGHTS_ORDER__SRC_H__
#define AT_DISTR_WEIGHTS_ORDER__SRC_H__


#include "at_distr_weights.h"
#include "../domain/at_distr_domain.h"
#include "at_distr_weights_order.h"


at_bool_t at_distr_weights__diff(
    at_distr_weights_t *w1,
    at_distr_weights_t *w2)
{
  if (w1->mode != w2->mode) {
    return AT__TRUE;
  } else if (w1->q_order != w2->q_order) {
    return AT__TRUE;
  }

  if (w1->mode == AT_DISTR_WEIGHTS_MODE__GAUSSIAN) {
    return (w1->beta0 != w2->beta0) || (w1->sigma != w2->sigma);
  } else if (w1->mode == AT_DISTR_WEIGHTS_MODE__EXPONENTIAL) {
    return w1->c != w2->c;
  } else {
    return at_distr_weights_composite__diff(
        w1->composite, w2->composite);
  }

  return AT__FALSE;
}



int at_distr_weights__compare(
    at_distr_weights_t *w1,
    at_distr_weights_t *w2)
{
  if (w1->q_order < w2->q_order) {
    return -1;
  } else if (w1->q_order > w2->q_order) {
    return 1;
  } else {
    return 0;
  }
}


/* calculating the mean beta */
static double at_distr_weights__calc_mean_beta(at_distr_weights_t *w)
{
  int i;
  double denom = 0.0;
  double numer = 0.0;
  double dbeta = (w->beta_max - w->beta_min) / w->n;

  for (i = 0; i < w->n; i++) {
    double beta = w->beta_min + (i + 0.5) * dbeta;
    double invwf = at_distr_weights__calc_inv_weight_bounded(
        w, beta, NULL, NULL, NULL);
    double wf = 1.0/invwf;
    denom += wf;
    numer += wf * beta;
  }

  return (denom > 0.0) ? (numer / denom) : 0.0;
}


/* calculating the ordering parameter q_order

   The ordering parameter defined here is equal
   to the difference from the maximal beta to
   the mean beta

   So it goes up with the mean temperature.
 */
double at_distr_weights__calc_q_order(at_distr_weights_t *w)
{
  double beta_mean = at_distr_weights__calc_mean_beta(w);

  w->q_order = w->beta_max - beta_mean;

  return w->q_order;
}


#endif
