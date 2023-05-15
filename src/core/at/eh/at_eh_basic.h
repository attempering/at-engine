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

#ifndef AT_EH_BASIC_H__
#define AT_EH_BASIC_H__

#include "at_eh__def.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;

/* initialize an object of at_eh_t
 * The initial values are taken from configuration file `cfg`
 * when available, otherwise default values are assumed */
int at_eh__cfg_init(at_eh_t *eh,
    at_mb_t *mb, zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose);

/* clear data */
void at_eh__clear(at_eh_t *eh);

void at_eh__finish(at_eh_t *eh);

void at_eh__manifest(const at_eh_t* eh, at_utils_manifest_t *manifest);

#endif
