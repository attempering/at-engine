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


int at_driver_langevin__conf_init(
    at_driver_langevin_t *langevin,
    at_distr_t *distr,
    at_mb_t *mb,
    at_utils_conf_t *conf,
    uint32_t rng_seed)
{
  langevin->distr = distr;

  langevin->mb = mb;

  at_utils_logger__init_delegate(langevin->logger, conf->logger, "at.driver.langevin");

  at_utils_conf__push_mod(conf, "at.driver.langevin");

  /* dt: time step for the temperature Langevin eq */
  at_utils_conf__get_double(conf,
      "Tdt,T-dt,langevin-T-dt,langevin-dt",
      &langevin->dt, 1e-5,
      "dt");

  /* dTmax: maximal amount of temperature change in a step */
  at_utils_conf__get_double(conf,
      "dTmax,dT-max,langevin-max-dT,langevin-dT-max",
      &langevin->dTmax, 1.0,
      "dTmax");

  /* whether to apply the Metropolisation correction */
  at_utils_conf__get_bool(conf,
      "langevin-corrected",
      &langevin->corrected, AT__TRUE,
      "corrected");

  /* whether to avoid crossing over unvisited bins */
  at_utils_conf__get_bool(conf,
      "langevin-no-skip",
      &langevin->no_skip, AT__TRUE,
      "no_skip");

  /* minimum number of visits before moving out of a bin */
  at_utils_conf__get_double(conf,
      "langevin-bin-min-visits",
      &langevin->bin_min_visits, 1.0,
      "bin_min_visits");

#ifdef AT_DRIVER_LANGEVIN__U_BIAS
  at_utils_conf__get_double(conf,
      "langevin-u-bias",
      &langevin->u_bias__, 0.0,
      "u_bias");
#endif

#ifdef AT_DRIVER_LANGEVIN__CORR_BIN_MIN_VISITS
  at_utils_conf__get_double(conf,
      "langevin-corr-bin-min-visits",
      &langevin->corr_bin_min_visits, langevin->bin_min_visits,
      "corr_bin_min_visits");
#endif

  at_utils_conf__get_int(conf,
      "langevin-nst-suggest,langevin-nst-suggestion",
      &langevin->nst_suggest, 100000,
      "nst_suggest");

  /* number of rejected Langevin moves */
  langevin->rejects = 0.0;
  /* total number of (corrected) Langevin moves */
  langevin->total = 0.0;
  /* number of moves exempted from corrections */
  langevin->n_exemption = 0.0;

  /* initialize the integrator */
  {
    at_bool_t use_zerofiller = !langevin->no_skip;

    at_driver_langevin_integrator__init(
        langevin->integrator,
        mb->distr,
        mb,
        use_zerofiller,
        AT__FALSE, 0.0);
  }

  /* the custom integration function must be set manually */
  langevin->integrate_func = NULL;

  // initialize the random number generator
  at_driver_langevin_rng__conf_init(langevin->rng, conf, rng_seed);

  at_utils_conf__get_filename(conf,
      "langevin-file",
      &langevin->file, "langevin.dat",
      "file");

  at_utils_conf__pop_mod(conf);

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
  at_utils_logger__finish(langevin->logger);
  memset(langevin, 0, sizeof(*langevin));
}


void at_driver_langevin__manifest(const at_driver_langevin_t *langevin, at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.driver.langevin");

  at_utils_manifest__print_str(manifest, langevin->file, "file", "langevin-file");

  at_utils_manifest__print_double(manifest, langevin->dt, "dt", "langevin-dt");

  at_utils_manifest__print_double(manifest, langevin->dTmax, "dTmax", "langevin-dT-max");

  at_utils_manifest__print_double(manifest, langevin->rejects, "rejects", NULL);

  at_utils_manifest__print_double(manifest, langevin->total, "total", NULL);

  at_utils_manifest__print_bool(manifest, langevin->corrected, "corrected", "langevin-corrected");

  at_utils_manifest__print_bool(manifest, langevin->no_skip, "no_skip", "langevin-no-skip");

  at_utils_manifest__print_double(manifest, langevin->bin_min_visits, "bin_min_visits", "langevin-bin-min-visits");

#ifdef AT_DRIVER_LANGEVIN__U_BIAS
  at_utils_manifest__print_double(manifest, langevin->u_bias__, "u_bias__", "langevin-u-bias");
#endif

#ifdef AT_DRIVER_LANGEVIN__CORR_BIN_MIN_VISITS
  at_utils_manifest__print_double(manifest, langevin->corr_bin_min_visits, "corr_bin_min_visits", "langevin-corr-bin-min-visits");
#endif

  at_utils_manifest__print_int(manifest, langevin->nst_suggest, "nst_suggest", "langevin-nst-suggestion");

  at_utils_manifest__pop_mod(manifest);
}



#endif
