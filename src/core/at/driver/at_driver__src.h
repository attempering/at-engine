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

#ifndef AT_DRIVER__SRC_H__
#define AT_DRIVER__SRC_H__


#include "at_driver.h"

#include "langevin/at_driver_langevin__src.h"

#include "../utils/at_utils.h"
#include "../distr/at_distr.h"
#include "../mb/at_mb.h"
#include "../../zcom/zcom.h"



int at_driver__cfg_init(
    at_driver_t *driver,
    at_distr_t *distr,
    at_mb_t *mb,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  at_driver_langevin__cfg_init(
    driver->langevin, distr, mb,
    cfg, ssm, data_dir, verbose);

  /* nsttemp: frequency of tempering, 0: disable, -1: only ns */
  driver->nsttemp = -1;
  if (zcom_cfg__get(cfg, &driver->nsttemp, "nsttemp,nst-temp,nst-tempering", "%d") != 0) {
    if (verbose) fprintf(stderr, "Info@at.driver: assuming default driver->nsttemp = -1, key: nst-tempering\n");
  }

  driver->move_repeats = 1;
  if (zcom_cfg__get(cfg, &driver->move_repeats, "move-repeats", "%d") != 0) {
    if (verbose) fprintf(stderr, "Info@at.driver: assuming default driver->move_repeats = 1, key: move-repeats\n");
  }

  return 0;
}



void at_driver__finish(at_driver_t *driver)
{
  at_driver_langevin__finish(driver->langevin);
}



void at_driver__manifest(
    const at_driver_t *driver,
    at_utils_manifest_t *manifest)
{
  at_driver_langevin__manifest(driver->langevin, manifest);

  at_utils_manifest__print_int(manifest, driver->nsttemp, "driver->nsttemp", "nst-tempering");

  at_utils_manifest__print_int(manifest, driver->move_repeats, "driver->move_repeats", "move-repeats");
}


int at_driver__read(at_driver_t *driver)
{
  return at_driver_langevin__read(driver->langevin);
}


int at_driver__write(at_driver_t *driver)
{
  return at_driver_langevin__write(driver->langevin);
}


#endif
