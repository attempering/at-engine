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

#ifndef AT_DISTR_WEIGHTS_H__
#define AT_DISTR_WEIGHTS_H__


#include "at_distr_weights__def.h"
#include "components/at_distr_weights_components.h"
#include "../domain/at_distr_domain__def.h"
#include "../../utils/at_utils.h"
#include "at_distr_weights_order.h"



typedef struct zcom_cfg_t_ zcom_cfg_t;

int at_distr_weights__conf_init(
    at_distr_weights_t *w,
    at_distr_domain_t *domain,
    at_utils_conf_t *conf);

void at_distr_weights__finish(at_distr_weights_t *w);

void at_distr_weights__manifest(
    const at_distr_weights_t *w,
    at_utils_manifest_t *manifest);

int at_distr_weights__get_n_components(
    const at_distr_weights_t *w);

void at_distr_weights__init_ens_exp(
    at_distr_weights_t *w);

void at_distr_weights__init_ens_w(
    at_distr_weights_t *w,
    at_distr_domain_t *domain);

/* return the inverse of overall temperature weight
 *
 * The weight is a product of two components, f and w factor
 * 
 * The f factor is 1 if mb->mode == 0,
 * or a Gaussian in the beta space if mb->mode == 1.
 * 
 * The w factor is proportional to beta^{ - mb->ens_exp }. 
 * 
 * The return value is the inverse of the product of the two factors.
 * 
 * The value of the f factor and its negative derivative w.r.t. beta
 * are saved in *p_f and *p_neg_df_dbeta, respectively,
 * if the two pointers are not NULL.
 */
double at_distr_weights__calc_inv_weight_simple(
    const at_distr_weights_t *w,
    double beta,
    double *neg_dlnwf_dbeta,
    double *f,
    double *neg_dlnf_dbeta);

double at_distr_weights__calc_inv_weight_bounded(
    const at_distr_weights_t *w,
    double beta,
    double *neg_dlnwf_dbeta,
    double *f,
    double *neg_dlnf_dbeta);


#endif
