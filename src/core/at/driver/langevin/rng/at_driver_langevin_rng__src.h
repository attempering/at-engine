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


void at_driver_langevin_rng__reset(
    at_driver_langevin_rng_t *rng, uint32_t seed,
    zcom_ssm_t *ssm, const char *data_dir, const char *fn_backup)
{
  int force_reload = 0;
  char* fn_rng_backup = NULL;

  if (rng->mtrng == NULL) {

    rng->mtrng = zcom_mtrng__open(seed);

  }

  // calculate the name of the backup file
  if (ssm != NULL && fn_backup != NULL) {

    if (data_dir != NULL) {

      fn_rng_backup = zcom_ssm__dup(ssm, data_dir);
      zcom_ssm__concat(ssm, &fn_rng_backup, "/");
      zcom_ssm__concat(ssm, &fn_rng_backup, fn_backup);

    } else {

      fn_rng_backup = zcom_ssm__dup(ssm, fn_backup);

    }

  }

  zcom_mtrng__load_or_init_from_seed(rng->mtrng,
      rng->file, seed, force_reload,
      fn_rng_backup);

  rng->ready = AT__TRUE;
}



int at_driver_langevin_rng__save(at_driver_langevin_rng_t *rng)
{
  if (rng->mtrng) {
    zcom_mtrng__save(rng->mtrng, rng->file);
  }

  return 0;
}



void at_driver_langevin_rng__conf_init(
    at_driver_langevin_rng_t *rng,
    at_utils_conf_t *conf)
{
  at_utils_conf__push_mod(conf, "at.driver.langevin.rng");

  at_utils_conf__get_filename(conf,
      "rngfile,rng-file,langevin-rng-file",
      &rng->file, "langevin-rng.dat",
      "file");

  at_utils_conf__pop_mod(conf);

  rng->mtrng = NULL;

  at_driver_langevin_rng__reset(rng, 0, conf->ssm, conf->data_dir, "langevin-rng-init.dat");
}



void at_driver_langevin_rng__finish(at_driver_langevin_rng_t *rng)
{
  if (rng->ready) {
    if (rng->mtrng != NULL) {
      zcom_mtrng__save(rng->mtrng, rng->file);
      zcom_mtrng__close(rng->mtrng);
    }
  }
}

void at_driver_langevin_rng__manifest(at_driver_langevin_rng_t *rng,
    at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.driver.langevin.rng");

  at_utils_manifest__print_str(manifest, rng->file, "file", "langevin-rng-file");

  at_utils_manifest__pop_mod(manifest);
}


#endif
