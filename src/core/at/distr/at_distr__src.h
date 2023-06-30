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

#include "../../zcom/zcom.h"



int at_distr__conf_init(at_distr_t *distr, at_utils_conf_t *conf, double boltz)
{
  distr->boltz = boltz;

  if (at_distr_domain__conf_init(distr->domain, conf) != 0) {
    return -1;
  }

  if (at_distr_weights__conf_init(distr->weights, distr->domain, conf) != 0) {
    return -1;
  }

  if (at_distr_bias__conf_init(distr->bias, conf) != 0) {
    return -1;
  }

  return 0;
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


double at_distr__calc_inv_weight(const at_distr_t *distr, double beta,
    double *neg_dlnwf_dbeta, double *f, double *neg_df_dbeta)
{
  return at_distr_weights__calc_inv_weight(distr->weights, beta,
      neg_dlnwf_dbeta, f, neg_df_dbeta);
}

#endif
