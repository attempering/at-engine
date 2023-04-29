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

int at_distr__cfg_init(at_distr_t *distr, zcom_cfg_t *cfg, int silent)
{
  at_distr_domain__cfg_init(distr->domain, cfg, silent);
  at_distr_weights__cfg_init(distr->weights, distr->domain, cfg, silent);
  at_distr_bias__cfg_init(distr->bias, cfg, silent);

  return 0;
}



void at_distr__finish(at_distr_t *distr)
{
  at_distr_bias__finish(distr->bias);
  at_distr_weights__finish(distr->weights);
  at_distr_domain__finish(distr->domain);
}



void at_distr__manifest(at_distr_t *distr, at_utils_manifest_t *manifest)
{
  at_distr_domain__manifest(distr->domain, manifest);
  at_distr_weights__manifest(distr->weights, manifest);
  at_distr_bias__manifest(distr->bias, manifest);
}



#endif
