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

#ifndef AT_BASIC_H__
#define AT_BASIC_H__

#include "at__def.h"


double at__beta_to_temp(const at_t *at, double beta);

double at__temp_to_beta(const at_t *at, double temp);


int at__load_data(at_t *at, at_bool_t is_continuation);


#define AT__INIT_VERBOSE 0x0001
//#define AT__INIT_OPENLOG 0x0002


/* initialize members of at_t from the configuration
 * file `cfg`, or if unavailable, from default values */
int at__cfg_init(at_t *at,
    zcom_cfg_t *cfg,
    const at_params_sys_t *sys_params,
    at_flags_t flags);

/* wrapper of at__cfg_init() */
int at__init(at_t *at,
    const char *cfg_filename,
    const at_params_sys_t *sys_params,
    at_flags_t flags);

/* return a pointer of an initialized at_t object
 * if possible, initial values are taken from configuration
 * file `cfg_filename`, otherwise default values are assumed */
at_t *at__open(const char *cfg_filename,
    const at_params_sys_t *sys_params,
    at_flags_t flags);

void at__finish(at_t *at);

/* close a pointer to at_t */
void at__close(at_t *at);

int at__manifest(at_t *at);



#endif
