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

int at_distr_bias__cfg_init(at_distr_bias_t *bias, zcom_cfg_t *cfg, at_bool_t verbose)
{
  /* enabled : 0: disable; 1:enable */
  bias->enabled = 0;
  if (zcom_cfg__get(cfg, &bias->enabled, "boost_mode", "%d") != 0)
  {
    fprintf(stderr, "Info: assuming default distr->bias->th_mode = 0, key: boost_mode\n");
  }

  if (bias->enabled) {

    /* ref_temp */
    bias->ref_temp = 300.0;
    if (zcom_cfg__get(cfg, &bias->ref_temp, "boost_Tref", "%lf") != 0)
    {
      fprintf(stderr, "Info: assuming default distr->bias->th_Tref = 300.0, key: boost_Tref\n");
    }

#ifdef AT__PCST_COMPAT
    /* kappa0 */
    bias->kappa0 = 1.0;
    if (zcom_cfg__get(cfg, &bias->kappa0, "kappa0", "%lf") != 0) {
      fprintf(stderr, "Info: assuming default distr->bias->kappa0 = 1.0, key: kappa0\n");
    }

    /* epsilon0 */
    bias->epsilon0 = 0.0;
    if (zcom_cfg__get(cfg, &bias->epsilon0, "epsilon0", "%lf") != 0) {
      fprintf(stderr, "Info: assuming default distr->bias->epsilon0 = 0.0, key: epsilon0\n");
    }
#endif

  }


  return 0;

}


void at_distr_bias__finish(at_distr_bias_t *bias)
{
}

void at_distr_bias__manifest(const at_distr_bias_t *bias, at_utils_manifest_t *manifest)
{
  at_utils_manifest__print_bool(manifest, bias->enabled, "distr->bias->enabled", "boost_mode");

  if (bias->enabled) {
    at_utils_manifest__print_bool(manifest, bias->ref_temp, "distr->bias->ref_temp", "boost_Tref");
#ifdef AT__PCST_COMPAT
    at_utils_manifest__print_bool(manifest, bias->kappa0, "distr->bias->kappa0", "kappa0");
    at_utils_manifest__print_bool(manifest, bias->epsilon0, "distr->bias->epsilon0", "epsilon0");
#endif
  }
}


#endif
