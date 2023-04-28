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

#ifndef AT_UTILS_MANIFEST_H__
#define AT_UTILS_MANIFEST_H__

#include "at_utils_manifest__def.h"

void at_utils_manifest__cfg_init(at_utils_manifest_t *manifest,
    zcom_cfg_t *cfg, int silent);

int at_utils_manifest__open_file(at_utils_manifest_t *manifest);

void at_utils_manifest__close_file(at_utils_manifest_t *manifest);

void at_utils_manifest__finish(at_utils_manifest_t *manifest);

#endif
