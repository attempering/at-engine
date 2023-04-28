/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

#ifndef AT_UTILS_H__
#define AT_UTILS_H__

#include "at_basic.h"

at_t *at__open(const char *zcom_cfg_fn, at_bool_t bCPT, at_bool_t open_log, double boltz, double md_time_step, int suffix);

int at__move(at_t *at, llong_t step, at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush);

#endif
