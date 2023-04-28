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

#ifndef AT_MB_BASIC_H__
#define AT_MB_BASIC_H__

/* basic routines for the multiple-bin estimators */

#include "at_mb__def.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;

/* initialize an object of at_mb_t
 * The initial values are taken from configuration file `cfg`
 * when available, otherwise default values are assumed */
int at_mb__cfg_init(at_mb_t *mb, zcom_cfg_t *cfg,
                double boltz,
                const char *data_dir,
                int silent);

/* destroy data members */
void at_mb__finish(at_mb_t *mb);

/* clear data members */
void at_mb__clear(at_mb_t *mb);

#endif
