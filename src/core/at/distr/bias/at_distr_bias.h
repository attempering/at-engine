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

#ifndef AT_DISTR_BIAS_H__
#define AT_DISTR_BIAS_H__

#include "at_distr_bias__def.h"
#include "../../utils/at_utils.h"

int at_distr_bias__conf_init(
    at_distr_bias_t *bias,
    at_utils_conf_t *conf,
    double ref_temp,
    double max_temp);

void at_distr_bias__finish(at_distr_bias_t *bias);

void at_distr_bias__manifest(const at_distr_bias_t *bias, at_utils_manifest_t *manifest);

/* get the bias weight for combination, as in

    U = U0 + w_comb * V
*/
double at_distr_bias__get_w_comb_of_temp(const at_distr_bias_t *bias, double temp);

/* get the bias weight for beta differentiation

    d(beta U)/d(beta) = U0 + w_deriv * V
*/
double at_distr_bias__get_w_deriv_of_temp(const at_distr_bias_t *bias, double temp);


#endif
