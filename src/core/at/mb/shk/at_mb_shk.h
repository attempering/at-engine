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

#ifndef AT_MB_SHK_H__
#define AT_MB_SHK_H__

/* shrinking rate of the adaptive averaging scheme */

#include <stdio.h>

#include "at_mb_shk__def.h"


typedef struct at_mb_t_ at_mb_t;
typedef struct zcom_cfg_t_ zcom_cfg_t;

int at_mb_shk__cfg_init(at_mb_shk_t *shk, zcom_cfg_t *cfg, at_mb_t *mb, int m, int silent);

void at_mb_shk__finish(at_mb_shk_t *shk);

void at_mb_shk__manifest(const at_mb_shk_t *shk, FILE *fp, int arrmax);

double at_mb_shk__calc_shk_base(at_mb_shk_t *shk, double total_visits);

double at_mb_shk__calc_inverse_gamma(at_mb_shk_t *shk, double total_visits, int ib);

#endif
