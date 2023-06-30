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

#ifndef AT_DRIVER_H__
#define AT_DRIVER_H__

#include "at_driver__def.h"
#include "../distr/at_distr.h"

#include "langevin/at_driver_langevin.h"

#include "../utils/at_utils.h"

int at_driver__conf_init(
    at_driver_t *driver,
    at_distr_t *distr,
    at_mb_t *mb,
    at_utils_conf_t *conf);

void at_driver__finish(at_driver_t *driver);

void at_driver__manifest(
    const at_driver_t *driver,
    at_utils_manifest_t *manifest);

int at_driver__read(at_driver_t *driver);

int at_driver__write(at_driver_t *driver);

#endif
