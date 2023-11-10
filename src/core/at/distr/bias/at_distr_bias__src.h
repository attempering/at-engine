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

#ifndef AT_DISTR_BIAS__SRC_H__
#define AT_DISTR_BIAS__SRC_H__

#include "at_distr_bias.h"

int at_distr_bias__conf_init(
    at_distr_bias_t *bias,
    at_utils_conf_t *conf,
    double ref_temp,
    double max_temp)
{
  at_utils_conf__push_mod(conf, "at.distr.bias");

  /* enabled : 0: disable; 1:enable */
  at_utils_conf__get_bool(conf,
      "bias-enabled,boost-mode",
      &bias->enabled, AT__FALSE,
      "enabled");

  if (bias->enabled) {

    at_utils_conf__get_double(conf,
        "bias-ref-temp,bias-Tref,boost-Tref",
        &bias->ref_temp,
        ref_temp,
        "ref_temp");

    at_utils_conf__get_double(conf,
        "bias-max-temp,bias-Tmax",
        &bias->max_temp,
        max_temp,
        "max_temp");

    at_utils_conf__get_double(conf,
        "bias-w-max",
        &bias->w_max,
        1.0,
        "w_max");

  }

  at_utils_conf__pop_mod(conf);

  return 0;

}



void at_distr_bias__finish(at_distr_bias_t *bias)
{
  bias->enabled = AT__FALSE;
}



void at_distr_bias__manifest(const at_distr_bias_t *bias, at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.distr.bias");

  at_utils_manifest__print_bool(manifest, bias->enabled, "enabled", "bias-enabled");

  if (bias->enabled) {
    at_utils_manifest__print_bool(manifest, bias->ref_temp, "ref_temp", "bias-ref-temp");
    at_utils_manifest__print_bool(manifest, bias->max_temp, "max_temp", "bias-max-temp");
    at_utils_manifest__print_bool(manifest, bias->w_max, "w_max", "bias-w-max");
  }

  at_utils_manifest__pop_mod(manifest);
}


double at_distr_bias__get_w_comb_of_temp(const at_distr_bias_t *bias, double temp)
{
  return bias->w_max * (temp - bias->ref_temp) / (bias->max_temp - bias->ref_temp);
}


double at_distr_bias__get_w_deriv_of_temp(const at_distr_bias_t *bias, double temp)
{
  (void) temp;
  return -bias->ref_temp * bias->w_max / (bias->max_temp - bias->ref_temp);
}



#endif
