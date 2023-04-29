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

#ifndef AT_DISTR_H__
#define AT_DISTR_H__

#include "at_distr__def.h"

#include "domain/at_distr_domain.h"
#include "weights/at_distr_weights.h"
#include "bias/at_distr_bias.h"

#include "../utils/at_utils.h"

int at_distr__cfg_init(at_distr_t *distr, double boltz, zcom_cfg_t *cfg, int silent);

void at_distr__finish(at_distr_t *distr);

void at_distr__manifest(at_distr_t *distr, at_utils_manifest_t *manifest);

int at_distr__beta_to_index(at_distr_t *distr, double beta, int check);

double at_distr__calc_inv_weight(at_distr_t *distr, double beta,
    double *neg_dlnwf_dbeta, double *f, double *neg_df_dbeta);

#endif
