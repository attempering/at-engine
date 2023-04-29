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

#ifndef AT_MB_IIE_GRIDVALS_BASIC_H__
#define AT_MB_IIE_GRIDVALS_BASIC_H__


#include "at_mb_iie_gridvals__def.h"

typedef struct at_mb_t_ at_mb_t;
typedef struct zcom_cfg_t_ zcom_cfg_t;


void at_mb_iie_gridvals__cfg_init(at_mb_iie_gridvals_t *gridvals, at_mb_t *mb,
    zcom_cfg_t *cfg, int verbose);

void at_mb_iie_gridvals__clear(at_mb_iie_gridvals_t *gridvals);

void at_mb_iie_gridvals__finish(at_mb_iie_gridvals_t *gridvals);


#endif
