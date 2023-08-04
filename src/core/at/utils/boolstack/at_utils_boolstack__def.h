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

#ifndef AT_UTILS_BOOLSTACK__DEF_H__
#define AT_UTILS_BOOLSTACK__DEF_H__

#include "../../context/at_context__def.h"

#define AT_UTILS_BOOLSTACK__STACK_MAX 256

typedef struct at_utils_boolstack_t_
{
  int curr;
  at_bool_t stack[AT_UTILS_BOOLSTACK__STACK_MAX];
} at_utils_boolstack_t;

#endif
