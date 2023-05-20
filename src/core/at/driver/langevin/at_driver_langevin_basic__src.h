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
    at_bool_t verbose)
{
  langevin->distr = distr;

  langevin->mb = mb;

  /* dt: time step for the temperature Langevin eq */
  langevin->dt = 1e-5;
  if (0 != zcom_cfg__get(cfg, &langevin->dt, "Tdt", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default driver->langevin->dt = 1e-5, key: Tdt\n");
  }

  /* dTmax: maximal amount of temperature change in a step */
  langevin->dTmax = 25.0;
  if (0 != zcom_cfg__get(cfg, &langevin->dTmax, "dTmax", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default driver->langevin->dTmax = 25.0, key: dTmax\n");
  }

  /* whether to apply the Metropolisation correction */
  langevin->corrected = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->corrected, "langevin_corrected", "%d")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default driver->langevin->corrected = 1, key: langevin_corrected\n");
  }

  /* whether to avoid crossing over unvisited bins */
  langevin->no_skip = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->no_skip, "langevin_no_skip", "%d")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default driver->langevin->no_skip = 1, key: langevin_no_skip\n");
  }

  /* minimum number of visits before moving out of a bin */
  langevin->bin_min_visits = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->bin_min_visits, "langevin_bin_min_visits", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default driver->langevin->bin_min_visits = 1, key: langevin_bin_min_visits\n");
  }

  langevin->nst_suggest = 100000;
  if (zcom_cfg__get(cfg, &langevin->nst_suggest, "langevin_nst_suggest", "%d") != 0) {
    if (verbose) fprintf(stderr, "Info@at: assuming default langevin->nst_suggest = %d, key: langevin_nst_suggest\n",
        langevin->nst_suggest);
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

  {
    char *fn = (char *) "langevin.dat";
    if (0 != zcom_cfg__get(cfg, &langevin->file, "langevin_file", "%s")) {
      if (verbose) fprintf(stderr, "Info@at: assuming default langevin->file = \"%s\", key: langevin_file\n", fn);
    }
    langevin->file = at_utils__make_output_filename(ssm, data_dir, fn);
  }

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
  at_utils_manifest__print_double(manifest, langevin->dt, "driver->langevin->dt", "Tdt");

  at_utils_manifest__print_double(manifest, langevin->dTmax, "driver->langevin->dTmax", "dTmax");

  at_utils_manifest__print_double(manifest, langevin->rejects, "driver->langevin->rejects", NULL);

  at_utils_manifest__print_double(manifest, langevin->total, "driver->langevin->total", NULL);

  at_utils_manifest__print_bool(manifest, langevin->corrected, "driver->langevin->corrected", "langevin_corrected");

  at_utils_manifest__print_bool(manifest, langevin->no_skip, "driver->langevin->no_skip", "langevin_no_skip");

  at_utils_manifest__print_double(manifest, langevin->bin_min_visits, "driver->langevin->bin_min_visits", "langevin_bin_min_visits");

  at_utils_manifest__print_int(manifest, langevin->nst_suggest, "driver->langevin->nst_suggest", "langevin_nst_suggest");
}



#endif
