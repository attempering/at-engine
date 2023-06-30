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

#ifndef AT_MB_BASIC_H__
#define AT_MB_BASIC_H__

/* basic routines for the multiple-bin estimators */

#include "at_mb__def.h"

#include "../distr/at_distr.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;

int at_mb__conf_init(
    at_mb_t *mb,
    at_distr_t *distr,
    at_utils_conf_t *conf,
    double boltz);

void at_mb__finish(at_mb_t *mb);

void at_mb__clear(at_mb_t *mb);

void at_mb__manifest(const at_mb_t *mb, at_utils_manifest_t *manifest);

#endif
