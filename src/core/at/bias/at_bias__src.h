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

#ifndef AT_BIAS__SRC_H__
#define AT_BIAS__SRC_H__

#include "at_bias.h"
#include "../../zcom/zcom.h"

void at_bias__cfg_init(at_bias_t *bias, zcom_cfg_t *cfg, int silent)
{
  /* bTH : 0: disable; 1:enable */
  bias->bTH = 0;
  if (cfg != NULL && zcom_cfg__get(cfg, &bias->bTH, "boost_mode", "%d"))
  {
    fprintf(stderr, "assuming default: bias->th_mode = 0, key: boost_mode\n");
  }

  if (bias->bTH) {

    /* TH_Tref */
    bias->TH_Tref = 300.0;
    if (cfg != NULL && zcom_cfg__get(cfg, &bias->TH_Tref, "boost_Tref", "%lf"))
    {
      fprintf(stderr, "assuming default: bias->th_Tref = 300.0, key: boost_Tref\n");
    }

    /* kappa0 */
    bias->kappa0 = 1.0;
    if (cfg != NULL && zcom_cfg__get(cfg, &bias->kappa0, "kappa0", "%lf")) {
      fprintf(stderr, "assuming default: bias->kappa0 = 1.0, key: kappa0\n");
    }

    /* epsilon0 */
    bias->epsilon0 = 0.0;
    if (cfg != NULL && zcom_cfg__get(cfg, &bias->epsilon0, "epsilon0", "%lf")) {
      fprintf(stderr, "assuming default: bias->epsilon0 = 0.0, key: epsilon0\n");
    }

  }

}


void at_bias__finish(at_bias_t *bias)
{
}

void at_bias__manifest(at_bias_t *bias, FILE *fp, int arrmax)
{
  fprintf(fp, "bias->bTH: at_bool_t, %s\n", (bias->bTH ? "true" : "false"));

  if (bias->bTH) {
    fprintf(fp, "bias->th_Tref: double, %g\n", bias->TH_Tref);
    fprintf(fp, "bias->kappa0: double, %g\n", bias->kappa0);
    fprintf(fp, "bias->epsilon0: double, %g\n", bias->epsilon0);
  }
}


#endif
