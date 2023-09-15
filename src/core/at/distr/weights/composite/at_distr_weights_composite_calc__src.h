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

#ifndef AT_DISTR_WEIGHTS_COMPOSITE_CALC__SRC_H__
#define AT_DISTR_WEIGHTS_COMPOSITE_CALC__SRC_H__


#include "at_distr_weights_composite.h"
#include "../../domain/at_distr_domain.h"

#include "../../../utils/at_utils.h"



double at_distr_weights_composite__calc_f_factor_simple(
    const at_distr_weights_composite_t *c,
    double beta,
    double *neg_dlnf_dbeta,
    at_utils_logger_t *logger)
{
  double f, neg_df_dbeta;
  int ic;

  if (c->n_components > 0) {

    f = 0.0;
    neg_df_dbeta = 0.0;

    for (ic = 0; ic < c->n_components; ic++) {
      double f_comp, neg_dlnf_dbeta_comp;

      f_comp = at_distr_weights_component__calc_f_factor_simple(
          c->components + ic, beta, &neg_dlnf_dbeta_comp, logger);
      //printf("wc-simple: comp %d, %g %g\n", ic, f_comp, neg_dlnf_dbeta_comp);

      f += f_comp;
      neg_df_dbeta += f_comp * neg_dlnf_dbeta_comp;

      //printf("wc-simple: comp %d, %g, %g\n", ic, f, neg_df_dbeta);

    }

  } else {

    f = 1.0;
    neg_df_dbeta = 0.0;

  }

  if (neg_dlnf_dbeta != NULL) {
    *neg_dlnf_dbeta = (f != 0.0) ? (neg_df_dbeta / f) : 0.0;
  }

  return f;
}




zcom_xdouble_t at_distr_weights_composite__calc_f_factor_unbounded(
    const at_distr_weights_composite_t *c,
    double beta,
    double *neg_dlnf_dbeta,
    at_utils_logger_t *logger)
{
  zcom_xdouble_t f, neg_df_dbeta;
  int ic;

  if (c->n_components > 0) {

    f = zcom_xdouble__from_double(0.0);
    neg_df_dbeta = zcom_xdouble__from_double(0.0);

    for (ic = 0; ic < c->n_components; ic++) {
      zcom_xdouble_t f_comp, neg_df_dbeta_comp;
      double neg_dlnf_dbeta_comp = 0.0;

      f_comp = at_distr_weights_component__calc_f_factor_unbounded(
          c->components + ic, beta, &neg_dlnf_dbeta_comp, logger);
      //printf("wc-unbounded: comp %d, %g, %g\n", ic, zcom_xdouble__to_double(f_comp), neg_dlnf_dbeta_comp);

      f = zcom_xdouble__add(f, f_comp);

      neg_df_dbeta_comp = zcom_xdouble__mul(
          f_comp,
          zcom_xdouble__from_double(neg_dlnf_dbeta_comp));

      neg_df_dbeta = zcom_xdouble__add(
          neg_df_dbeta, neg_df_dbeta_comp);

      //printf("wc-unbounded: comp %d, %g, %g\n", ic, zcom_xdouble__to_double(f), zcom_xdouble__to_double(neg_df_dbeta));
    }

  } else {

    f = zcom_xdouble__from_double(1.0);
    neg_df_dbeta = zcom_xdouble__from_double(0.0);

  }

  if (neg_dlnf_dbeta != NULL) {
    zcom_xdouble_t neg_dlnf_dbeta_ = zcom_xdouble__div(
        neg_df_dbeta, f);
    *neg_dlnf_dbeta = zcom_xdouble__to_double(neg_dlnf_dbeta_);
  }

  return f;
}


static double at_distr_weights_composite__get_ln_zero(void) {
  return -10000.0;
}

/* returns log(exp(x) + exp(y)) */
static double at_distr_weights_composite__ln_add(double x, double y) {
  if (x > y) {
    return x + log(1 + exp(y-x));
  } else {
    return y + log(1 + exp(x-y));
  }
}



double at_distr_weights_composite__calc_lnf(
    const at_distr_weights_composite_t *c,
    double beta,
    at_utils_logger_t *logger)
{
  double lnf = at_distr_weights_composite__get_ln_zero();

  if (c->n_components > 0) {

    int ic;

    for (ic = 0; ic < c->n_components; ic++) {

      double lnf_comp = at_distr_weights_component__calc_lnf(
          c->components + ic, beta, logger);

      lnf = at_distr_weights_composite__ln_add(lnf, lnf_comp);

    }

  }

  return lnf;
}



#endif
