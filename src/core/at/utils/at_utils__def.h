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

#ifndef AT_UTILS__DEF_H__
#define AT_UTILS__DEF_H__

#include "manifest/at_utils_manifest__def.h"
#include "trace/at_utils_trace__def.h"

typedef struct at_utils_t_
{
  int inited;
  at_utils_manifest_t manifest[1];
  at_utils_trace_t log[1];
  double temp_thermostat;
  zcom_ssm_t *ssm;
} at_utils_t;

#endif
