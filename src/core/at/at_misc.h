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

#ifndef AT_MISC_H__
#define AT_MISC_H__

#include "at__def.h"


double at__beta_to_temp(const at_t *at, double beta);

double at__temp_to_beta(const at_t *at, double temp);

void at__set_energy(at_t *at, double energy);

int at__load_data(at_t *at, at_bool_t is_continuation);

at_bool_t at__do_on_step(
    const at_params_step_t *step_params,
    at_llong_t nst,
    at_bool_t do_on_first_step);

void at__output(at_t *at,
    const at_params_step_t *step_params,
    int ib, double invw, double t1, double t2, double av_energy);


#endif

