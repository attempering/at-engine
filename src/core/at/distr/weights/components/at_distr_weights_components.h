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

#ifndef AT_DISTR_WEIGHTS_COMPONENTS_H__
#define AT_DISTR_WEIGHTS_COMPONENTS_H__

#include <stdio.h>

#include "at_distr_weights_components__def.h"
#include "../../domain/at_distr_domain__def.h"
#include "../../../utils/at_utils.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;

int at_distr_weights_components__conf_init(
    at_distr_weights_components_t *c,
    at_distr_domain_t *domain,
    at_utils_conf_t *conf);

void at_distr_weights_components__finish(
    at_distr_weights_components_t *c);

void at_distr_weights_components__manifest(
    const at_distr_weights_components_t *c,
    at_utils_manifest_t *manifest);

double at_distr_weights_components__calc_f_factor(
    const at_distr_weights_components_t *c,
    double beta, double *p_neg_df_dbeta,
    at_utils_log_t *log);

void at_distr_weights_components__basic_init(
    at_distr_weights_components_t *c,
    at_distr_domain_t *domain,
    int n_components);

#endif
