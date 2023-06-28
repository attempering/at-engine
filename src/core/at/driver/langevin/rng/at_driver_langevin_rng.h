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

#ifndef AT_DRIVER_LANGEVIN_RNG_H__
#define AT_DRIVER_LANGEVIN_RNG_H__

#include "at_driver_langevin_rng__def.h"
#include "../../../utils/at_utils.h"

void at_driver_langevin_rng__reset(at_driver_langevin_rng_t *rng, uint32_t seed,
    zcom_ssm_t *ssm, const char *data_dir, const char *fn_backup);

int at_driver_langevin_rng__save(at_driver_langevin_rng_t *rng);

void at_driver_langevin_rng__cfg_init(at_driver_langevin_rng_t *rng,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose);

void at_driver_langevin_rng__finish(at_driver_langevin_rng_t *rng);

void at_driver_langevin_rng__manifest(at_driver_langevin_rng_t *rng,
    at_utils_manifest_t *manifest);

#endif
