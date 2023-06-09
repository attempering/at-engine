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

#ifndef AT_UTILS_LOCKFILE_H__
#define AT_UTILS_LOCKFILE_H__

#include "at_utils_lockfile__def.h"
#include "../../../zcom/zcom.h"

int at_utils_lockfile__init(
    at_utils_lockfile_t *lockfile,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose);

void at_utils_lockfile__finish(
    at_utils_lockfile_t *lockfile);

#endif