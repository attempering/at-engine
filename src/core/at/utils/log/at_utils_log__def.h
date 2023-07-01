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

#ifndef AT_UTILS_LOG__DEF_H__
#define AT_UTILS_LOG__DEF_H__

#include "../../context/at_context__def.h"
#include "../modstack/at_utils_modstack__def.h"
#include "../../../zcom/zcom.h"

typedef struct at_utils_log_t_
{
  int ready;
  char *file;
  at_utils_modstack_t mods[1];
  FILE *fp;
  at_bool_t print_to_stderr;
} at_utils_log_t;

#endif
