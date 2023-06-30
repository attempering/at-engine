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
#include "../../../zcom/zcom.h"

int at_distr_bias__conf_init(at_distr_bias_t *bias, at_utils_conf_t *conf)
{
  /* enabled : 0: disable; 1:enable */
  at_utils_conf__get_bool(conf,
      "boost-mode",
      &bias->enabled, AT__FALSE,
      "mode");

  if (bias->enabled) {

    at_utils_conf__get_double(conf,
        "boost-Tref",
        &bias->ref_temp, 300.0,
        "ref_temp");

#ifdef AT__PCST_COMPAT
    at_utils_conf__get_double(conf,
        "kappa0",
        &bias->kappa0, 1.0,
        "kappa0");

    at_utils_conf__get_double(conf,
        "epsilon0",
        &bias->epsilon0, 0.0,
        "epsilon0");

#endif

  }


  return 0;

}


void at_distr_bias__finish(at_distr_bias_t *bias)
{
  bias->enabled = AT__FALSE;
}



void at_distr_bias__manifest(const at_distr_bias_t *bias, at_utils_manifest_t *manifest)
{
  at_utils_manifest__print_bool(manifest, bias->enabled, "distr->bias->enabled", "boost-mode");

  if (bias->enabled) {
    at_utils_manifest__print_bool(manifest, bias->ref_temp, "distr->bias->ref_temp", "boost-Tref");
#ifdef AT__PCST_COMPAT
    at_utils_manifest__print_bool(manifest, bias->kappa0, "distr->bias->kappa0", "kappa0");
    at_utils_manifest__print_bool(manifest, bias->epsilon0, "distr->bias->epsilon0", "epsilon0");
#endif
  }
}


#endif
