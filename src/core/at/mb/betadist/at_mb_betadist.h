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

#ifndef AT_MB_BETADIST_H__
#define AT_MB_BETADIST_H__

#include <stdio.h>

#include "at_mb_betadist__def.h"

typedef struct at_mb_t_ at_mb_t;
typedef struct zcom_cfg_t_ zcom_cfg_t;

int at_mb_betadist__cfg_init(at_mb_betadist_t *betadist, zcom_cfg_t *cfg, at_mb_t *mb);

void at_mb_betadist__finish(at_mb_betadist_t *betadist);

void at_mb_betadist__manifest(const at_mb_betadist_t *betadist, FILE *fp, int arrmax);

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
double at_mb_betadist__calc_inv_weight(at_mb_betadist_t *betadist, double beta,
    double *neg_dlnwf_dbeta, double *f, double *neg_df_dbeta);

#endif
