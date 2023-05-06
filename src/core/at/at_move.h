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

#ifndef AT_MOVE_H__
#define AT_MOVE_H__

#include "at_basic.h"

void at__update_force_scale(at_t *at);

at_bool_t at__do_tempering(at_t *at, at_llong_t step);

int at__move(at_t *at,
             const at_params_step_t *step_params);

// convenience wrapper of at__move()
int at__step(at_t *at,
             double energy,
             at_llong_t step,
             at_params_step_t *step_params);

#endif
