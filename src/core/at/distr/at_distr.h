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

int at_distr__conf_init(
    at_distr_t *distr,
    at_utils_conf_t *conf,
    double boltz,
    double ref_temp);

/* convenience initializer for testing programs
   assuming boltz = 1.0, ref_temp = 1.0 */
int at_distr__conf_init_ez_(
    at_distr_t *distr,
    at_utils_conf_t *conf);

void at_distr__finish(at_distr_t *distr);

void at_distr__manifest(const at_distr_t *distr, at_utils_manifest_t *manifest);

int at_distr__beta_to_index(const at_distr_t *distr, double beta, at_bool_t check);

double at_distr__get_bin_center(const at_distr_t *distr, int ib);

double at_distr__calc_inv_weight_simple(
    const at_distr_t *distr, double beta,
    double *neg_dlnwf_dbeta,
    double *f, double *neg_dlnf_dbeta);

double at_distr__calc_inv_weight_bounded(
    const at_distr_t *distr, double beta,
    double *neg_dlnwf_dbeta,
    double *f, double *neg_dlnf_dbeta);

/* get the bias weight for combination, as in

    U = U0 + w_comb * V

  Note: this function accepts beta, instead of T.
*/
double at_distr__get_bias_w_comb(const at_distr_t *distr, double beta);

/* get the bias weight for beta differentiation

    d(beta U)/d(beta) = U0 + w_deriv * V

  Note: this function accepts beta, instead of T.
*/
double at_distr__get_bias_w_deriv(const at_distr_t *distr, double beta);


#endif
