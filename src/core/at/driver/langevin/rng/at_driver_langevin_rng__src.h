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

#ifndef AT_DRIVER_LANGEVIN_RNG__SRC_H__
#define AT_DRIVER_LANGEVIN_RNG__SRC_H__

#include "at_driver_langevin_rng.h"

#include "../../../../zcom/zcom.h"



void at_driver_langevin_rng__reset(at_driver_langevin_rng_t *rng, uint32_t seed)
{
  int force_reload = 0;

  if (rng->mtrng == NULL) {
    rng->mtrng = zcom_mtrng__open(seed);
  }

  zcom_mtrng__load_or_init_from_seed(rng->mtrng, rng->filename, seed, force_reload);

  rng->inited = 1;
}


void at_driver_langevin_rng__save(at_driver_langevin_rng_t *rng)
{
  if (rng->mtrng) {
    zcom_mtrng__save(rng->mtrng, rng->filename);
  }
}


void at_driver_langevin_rng__cfg_init(at_driver_langevin_rng_t *rng,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  rng->filename = (char *) "rng.dat";
  if (zcom_cfg__get(cfg, &rng->filename, "rngfile,rng-file", "%s") != 0)
  {
    if (verbose) fprintf(stderr, "Info@at: assuming default langevin->rng->filename = \"rng.dat\", key: rng-file\n");
  }

  rng->filename = at_utils__make_output_filename(ssm, data_dir, rng->filename);

  rng->mtrng = NULL;

  at_driver_langevin_rng__reset(rng, 0);
}



void at_driver_langevin_rng__finish(at_driver_langevin_rng_t *rng)
{
  if (rng->inited) {
    if (rng->mtrng != NULL) {
      zcom_mtrng__save(rng->mtrng, rng->filename);
      zcom_mtrng__close(rng->mtrng);
    }
  }
}

void at_driver_langevin_rng__manifest(at_driver_langevin_rng_t *rng,
    at_utils_manifest_t *manifest)
{
  at_utils_manifest__print_str(manifest, rng->filename, "driver->langevin->rng->filename", "rng_file");
}


#endif
