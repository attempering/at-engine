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

#ifndef AT_IO_EXT_H___
#define AT_IO_EXT_H___

#include "../at__def.h"
#include "at_io_ext.h"


at_bool_t at__do_output_on_step(
    const at_params_step_t *step_params,
    at_llong_t nst,
    at_bool_t do_on_first_step);

void at__output(
    at_t *at,
    const at_params_step_t *step_params);

int at__load_data(at_t *at);


#endif
