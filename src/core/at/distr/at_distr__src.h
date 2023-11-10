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

#ifndef AT_DISTR__SRC_H__
#define AT_DISTR__SRC_H__


#include "at_distr.h"

#include "domain/at_distr_domain__src.h"
#include "weights/at_distr_weights__src.h"
#include "bias/at_distr_bias__src.h"



int at_distr__conf_init(
    at_distr_t *distr,
    at_utils_conf_t *conf,
    double boltz,
    double ref_temp)
{
  double max_temp;

  distr->boltz = boltz;

  if (at_distr_domain__conf_init(distr->domain, conf) != 0) {
    return -1;
  }

  if (at_distr_weights__conf_init(distr->weights, distr->domain, conf) != 0) {
    return -1;
  }

  max_temp = 1.0 / (boltz * distr->domain->beta_min);

  if (at_distr_bias__conf_init(distr->bias, conf, ref_temp, max_temp) != 0) {
    return -1;
  }

  return 0;
}



int at_distr__conf_init_ez_(
    at_distr_t *distr,
    at_utils_conf_t *conf)
{
  double boltz = 1.0;
  double ref_temp = 1.0;

  return at_distr__conf_init(distr, conf, boltz, ref_temp);
}



void at_distr__finish(at_distr_t *distr)
{
  at_distr_bias__finish(distr->bias);
  at_distr_weights__finish(distr->weights);
  at_distr_domain__finish(distr->domain);
}



void at_distr__manifest(const at_distr_t *distr, at_utils_manifest_t *manifest)
{
  at_distr_domain__manifest(distr->domain, manifest);
  at_distr_weights__manifest(distr->weights, manifest);
  at_distr_bias__manifest(distr->bias, manifest);
}



int at_distr__beta_to_index(const at_distr_t *distr, double beta, at_bool_t check)
{
  return at_distr_domain__beta_to_index(distr->domain, beta, check);
}


double at_distr__get_bin_center(const at_distr_t *distr, int ib)
{
  return at_distr_domain__get_bin_center(distr->domain, ib);
}


double at_distr__calc_inv_weight_simple(
    const at_distr_t *distr, double beta,
    double *neg_dlnwf_dbeta,
    double *f, double *neg_dlnf_dbeta)
{
  return at_distr_weights__calc_inv_weight_simple(
      distr->weights, beta,
      neg_dlnwf_dbeta,
      f, neg_dlnf_dbeta);
}


double at_distr__calc_inv_weight_bounded(
    const at_distr_t *distr, double beta,
    double *neg_dlnwf_dbeta,
    double *f, double *neg_dlnf_dbeta)
{
  return at_distr_weights__calc_inv_weight_bounded(
      distr->weights, beta,
      neg_dlnwf_dbeta,
      f, neg_dlnf_dbeta);
}


double at_distr__get_bias_w_comb(const at_distr_t *distr, double beta)
{
  return at_distr_bias__get_w_comb_of_temp(distr->bias, 1.0/(distr->boltz*beta));
}


double at_distr__get_bias_w_deriv(const at_distr_t *distr, double beta)
{
  (void) beta;
  return at_distr_bias__get_w_deriv_of_temp(distr->bias, 1.0/(distr->boltz*beta));
}


#endif
