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

#ifndef AT_MB_IIE_ET_BASIC_H__
#define AT_MB_IIE_ET_BASIC_H__

#include "at_mb_iie_et__def.h"

typedef struct at_mb_t_ at_mb_t;
typedef struct zcom_cfg_t_ zcom_cfg_t;

void at_mb_iie_et__cfg_init(at_mb_iie_et_t *et, at_mb_t *mb, zcom_cfg_t *cfg, int silent);

void at_mb_iie_et__finish(at_mb_iie_et_t *et);

void at_mb_iie_et__clear(at_mb_iie_et_t *et);

void at_mb_iie_et__manifest(const at_mb_iie_et_t *et, FILE *fp, int arrmax);

#endif
