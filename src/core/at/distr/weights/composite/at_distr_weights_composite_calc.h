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

#ifndef AT_DISTR_WEIGHTS_COMPOSITE_CALC_H__
#define AT_DISTR_WEIGHTS_COMPOSITE_CALC_H__

#include <stdio.h>

#include "at_distr_weights_composite__def.h"
#include "../../domain/at_distr_domain__def.h"
#include "../../../utils/at_utils.h"


double at_distr_weights_composite__calc_f_factor_simple(
    const at_distr_weights_composite_t *c,
    double beta, double *neg_dlnf_dbeta,
    at_utils_logger_t *logger);

/* compute the ensemble f factor
 *
 *  f: f(beta);
 *  *neg_dlnf_dbeta: -d ln[f(beta)] / d(beta);
 *
 */
zcom_xdouble_t at_distr_weights_composite__calc_f_factor_unbounded(
    const at_distr_weights_composite_t *c,
    double beta,
    double *neg_dlnf_dbeta,
    at_utils_logger_t *logger);



double at_distr_weights_composite__calc_lnf(
    const at_distr_weights_composite_t *c,
    double beta,
    at_utils_logger_t *logger);


#endif
