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

#ifndef AT_PARAMS_SYS__SRC_H__
#define AT_PARAMS_SYS__SRC_H__

#include "at_params_sys.h"
#include <stdio.h>



void at_params_sys__init(
    at_params_sys_t *sys_params_dest,
    const at_params_sys_t *sys_params_user)
{
  if (sys_params_user == NULL) {
    sys_params_dest->boltz = 1.0;
    sys_params_dest->sim_id = 0;
    sys_params_dest->add_suffix = AT__FALSE;
    sys_params_dest->md_time_step = 0.002;
  } else {
    *sys_params_dest = *sys_params_user;
  }
}



#endif
