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

#ifndef AT_DRIVER_LANGEVIN_BASIC__SRC_H__
#define AT_DRIVER_LANGEVIN_BASIC__SRC_H__


#include "at_driver_langevin_basic.h"

#include "rng/at_driver_langevin_rng.h"

#include "zerofiller/at_driver_langevin_zerofiller.h"

#include "integrator/at_driver_langevin_integrator.h"


int at_driver_langevin__cfg_init(
    at_driver_langevin_t *langevin,
    at_distr_t *distr,
    at_mb_t *mb,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    int verbose)
{
  langevin->distr = mb->distr;

  langevin->mb = mb;

  /* dt: time step for the temperature Langevin eq */
  langevin->dt = 1e-5;
  if (0 != zcom_cfg__get(cfg, &langevin->dt, "Tdt", "%lf")) {
    if (verbose) fprintf(stderr, "assuming default: driver->langevin->dt = 1e-5, key: Tdt\n");
  }

  /* dTmax: maximal amount of temperature change in a step */
  langevin->dTmax = 25.0;
  if (0 != zcom_cfg__get(cfg, &langevin->dTmax, "dTmax", "%lf")) {
    if (verbose) fprintf(stderr, "assuming default: driver->langevin->dTmax = 25.0, key: dTmax\n");
  }

  /* whether to apply the Metropolisation correction */
  langevin->corrected = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->corrected, "langevin_corrected", "%d")) {
    if (verbose) fprintf(stderr, "assuming default: driver->langevin->corrected = 1, key: langevin_corrected\n");
  }

  /* whether to avoid crossing over unvisited bins */
  langevin->no_skip = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->corrected, "langevin_no_skip", "%d")) {
    if (verbose) fprintf(stderr, "assuming default: driver->langevin->no_skip = 1, key: langevin_no_skip\n");
  }

  /* minimum number of visits before moving out of a bin */
  langevin->bin_min_visits = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->bin_min_visits, "langevin_bin_min_visits", "%lf")) {
    if (verbose) fprintf(stderr, "assuming default: driver->langevin->bin_min_visits = 1, key: langevin_bin_min_visits\n");
  }

  /* rejects: number of rejected Langevin moves */
  langevin->rejects = 0.0;
  /* total: total number of Langevin moves */
  langevin->total = 0.0;

  // initialize the integrator
  {
    int use_zerofiller;

    if (langevin->no_skip) {
      use_zerofiller = 0;
    } else {
      use_zerofiller = 1;
    }

    at_driver_langevin_integrator__init(
        langevin->integrator,
        mb->distr,
        mb,
        use_zerofiller);
  }

  /* the custom integration function must be set manually */
  langevin->integrate_func = NULL;

  // initialize the random number generator
  at_driver_langevin_rng__cfg_init(langevin->rng, cfg, ssm, data_dir, verbose);

  return 0;
}



void at_driver_langevin__clear(at_driver_langevin_t *langevin)
{
  langevin->rejects = 0.0;
  langevin->total = 0.0;
}



void at_driver_langevin__finish(at_driver_langevin_t *langevin)
{
  at_driver_langevin_integrator__finish(langevin->integrator);
  at_driver_langevin_rng__finish(langevin->rng);
  memset(langevin, 0, sizeof(*langevin));
}


void at_driver_langevin__manifest(const at_driver_langevin_t *langevin, at_utils_manifest_t *manifest)
{
  FILE *fp = manifest->fp;

  fprintf(fp, "driver->langevin->dt: double, %g\n", langevin->dt);

  fprintf(fp, "driver->langevin->dTmax: double, %g\n", langevin->dTmax);

  fprintf(fp, "driver->langevin->rejects: double, %g\n", langevin->rejects);

  fprintf(fp, "driver->langevin->total: double, %g\n", langevin->total);

  fprintf(fp, "driver->langevin->corrected: int, %d\n", langevin->corrected);

  fprintf(fp, "driver->langevin->no_skip: int, %d\n", langevin->no_skip);

  fprintf(fp, "driver->langevin->bin_min_visits: double, %lf\n", langevin->bin_min_visits);

  fprintf(fp, "driver->langevin->integrate_func: %p\n", langevin->integrate_func);
}



#endif
