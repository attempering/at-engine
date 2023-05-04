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

#ifndef AT_PARAMS_SYS__DEF_H__
#define AT_PARAMS_SYS__DEF_H__

#include "../../context/at_context__def.h"

typedef struct at_params_sys_t_ {
  double boltz;
  double md_time_step;

  at_bool_t is_continuation; // continue from a previous run

  int id;
  
  // append the id to the data directory
  // e.g., data_dir would be "atdata0", "atdata1", ... instead of "atdata"
  at_bool_t multi_sims;

  // name of the data directory,
  // to be constructed from the program
  char data_dir[64];


} at_params_sys_t;


#endif
