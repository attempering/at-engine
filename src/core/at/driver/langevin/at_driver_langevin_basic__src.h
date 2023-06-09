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
  if (0 != zcom_cfg__get(cfg, &langevin->dt, "Tdt,T-dt,langevin-T-dt,langevin-dt", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at.driver.langevin: assuming default driver->langevin->dt = 1e-5, key: langevin-dt\n");
  }

  /* dTmax: maximal amount of temperature change in a step */
  langevin->dTmax = 25.0;
  if (0 != zcom_cfg__get(cfg, &langevin->dTmax, "dTmax,dT-max,langevin-max-dT,langevin-dT-max", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at.driver.langevin: assuming default driver->langevin->dTmax = 25.0, key: langevin-dT-max\n");
  }

  /* whether to apply the Metropolisation correction */
  langevin->corrected = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->corrected, "langevin-corrected", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.driver.langevin: assuming default driver->langevin->corrected = 1, key: langevin-corrected\n");
  }

  /* whether to avoid crossing over unvisited bins */
  langevin->no_skip = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->no_skip, "langevin-no-skip", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.driver.langevin: assuming default driver->langevin->no_skip = 1, key: langevin-no-skip\n");
  }

  /* minimum number of visits before moving out of a bin */
  langevin->bin_min_visits = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->bin_min_visits, "langevin-bin-min-visits", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at.driver.langevin: assuming default driver->langevin->bin_min_visits = %g, key: langevin-bin-min-visits\n",
        langevin->bin_min_visits);
  }

#ifdef AT_DRIVER_LANGEVIN__CORR_BIN_MIN_VISITS
  langevin->corr_bin_min_visits = langevin->bin_min_visits;
  if (0 != zcom_cfg__get(cfg, &langevin->corr_bin_min_visits, "langevin-corr-bin-min-visits", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at.driver.langevin: assuming default driver->langevin->corr_bin_min_visits = %g, key: langevin-corr-bin-min-visits\n",
        langevin->corr_bin_min_visits);
  }
#endif

  langevin->nst_suggest = 100000;
  if (zcom_cfg__get(cfg, &langevin->nst_suggest, "langevin-nst-suggest,langevin-nst-suggestion", "%d") != 0) {
    if (verbose) fprintf(stderr, "Info@at.driver.langevin: assuming default driver->langevin->nst_suggest = %d, key: langevin-nst-suggestion\n",
        langevin->nst_suggest);
  }

  /* number of rejected Langevin moves */
  langevin->rejects = 0.0;
  /* total number of (corrected) Langevin moves */
  langevin->total = 0.0;
  /* number of moves exempted from corrections */
  langevin->n_exemption = 0.0;

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
    char *fn = zcom_ssm__dup(ssm, "langevin.dat");
    if (0 != zcom_cfg__get(cfg, &langevin->file, "langevin-file", "%s")) {
      if (verbose) fprintf(stderr, "Info@at.driver.langevin: assuming default driver->langevin->file = \"%s\", key: langevin-file\n", fn);
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
  at_utils_manifest__print_str(manifest, langevin->file, "driver->langevin->file", "langevin-file");

  at_utils_manifest__print_double(manifest, langevin->dt, "driver->langevin->dt", "langevin-dt");

  at_utils_manifest__print_double(manifest, langevin->dTmax, "driver->langevin->dTmax", "langevin-dT-max");

  at_utils_manifest__print_double(manifest, langevin->rejects, "driver->langevin->rejects", NULL);

  at_utils_manifest__print_double(manifest, langevin->total, "driver->langevin->total", NULL);

  at_utils_manifest__print_bool(manifest, langevin->corrected, "driver->langevin->corrected", "langevin-corrected");

  at_utils_manifest__print_bool(manifest, langevin->no_skip, "driver->langevin->no_skip", "langevin-no-skip");

  at_utils_manifest__print_double(manifest, langevin->bin_min_visits, "driver->langevin->bin_min_visits", "langevin-bin-min-visits");

#ifdef AT_DRIVER_LANGEVIN__CORR_BIN_MIN_VISITS
  at_utils_manifest__print_double(manifest, langevin->corr_bin_min_visits, "driver->langevin->corr_bin_min_visits", "langevin-corr-bin-min-visits");
#endif

  at_utils_manifest__print_int(manifest, langevin->nst_suggest, "driver->langevin->nst_suggest", "langevin-nst-suggestion");
}



#endif
