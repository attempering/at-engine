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

int at__load_data(at_t *at, at_bool_t is_continuation);

int at__do_every(at_llong_t step, int nst, at_bool_t bfirst, at_bool_t blast);

void at__output(at_t *at, at_llong_t step,
    int ib, double invw, double t1, double t2, double Eav,
    at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush);

/* initialize members of at_t from the configuration
 * file `cfg`, or if unavailable, from default values */
int at__cfg_init(at_t *at, zcom_cfg_t *cfg, int isuffix,
    double boltz, double md_time_step, int verbose);

at_t *at__open(const char *cfg_fn,
    at_bool_t is_continuation, at_bool_t open_log,
    double boltz, double md_time_step, int suffix,
    int verbose);

void at__finish(at_t *at);

/* close a pointer to at_t */
void at__close(at_t *at);

int at__manifest(at_t *at);

double at__beta_to_temp(const at_t *at, double beta);

double at__temp_to_beta(const at_t *at, double temp);

#endif

