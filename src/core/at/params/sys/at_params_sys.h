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

#ifndef AT_PARAMS_SYS_H__
#define AT_PARAMS_SYS_H__

#include "at_params_sys__def.h"

/* initialize system parameters `sys_params_dest` from
 * the user-provided value `sys_params_user` (if not null),
 * or from the default values.
 *
 */
void at_params_sys__init(
    at_params_sys_t *sys_params_dest,
    const at_params_sys_t *sys_params_user);

#endif
