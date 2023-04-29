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

#ifndef AT_LANGEVIN_BASIC__SRC_H__
#define AT_LANGEVIN_BASIC__SRC_H__


#include "at_langevin_basic.h"

#include "rng/at_langevin_rng.h"

#include "zerofiller/at_langevin_zerofiller.h"

#include "integrator/at_langevin_integrator.h"

#include "../../zcom/zcom.h"



int at_langevin__cfg_init(at_langevin_t *langevin,
    at_mb_t *mb,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    int silent)
{
  /* dt: time step for the temperature Langevin eq */
  langevin->dt = 1e-5;
  if (0 != zcom_cfg__get(cfg, &langevin->dt, "Tdt", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->dt = 1e-5, key: Tdt\n");
  }

  /* dTmax: maximal amount of temperature change in a step */
  langevin->dTmax = 25.0;
  if (0 != zcom_cfg__get(cfg, &langevin->dTmax, "dTmax", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->dTmax = 25.0, key: dTmax\n");
  }

  langevin->move_repeats = 1;
  if (zcom_cfg__get(cfg, &langevin->move_repeats, "move_repeats", "%d")) {
    fprintf(stderr, "assuming default: langevin->move_repeats = 1, key: move_repeats\n");
  }

  /* whether to apply the Metropolisation correction */
  langevin->corrected = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->corrected, "langevin_corrected", "%d")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->corrected = 1, key: langevin_corrected\n");
  }

  /* whether to avoid crossing over unvisited bins */
  langevin->no_skip = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->corrected, "langevin_no_skip", "%d")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->no_skip = 1, key: langevin_no_skip\n");
  }

  /* minimum number of visits before moving out of a bin */
  langevin->bin_min_visits = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->bin_min_visits, "langevin_bin_min_visits", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->bin_min_visits = 1, key: langevin_bin_min_visits\n");
  }

  /* rejects: number of rejected Langevin moves */
  langevin->rejects = 0.0;
  /* total: total number of Langevin moves */
  langevin->total = 0.0;

  // initialize the random number generator
  at_langevin_rng__cfg_init(langevin->rng, cfg, ssm, data_dir, silent);

  // initialize the integrator
  {
    int use_zerofiller;

    if (langevin->no_skip) {
      use_zerofiller = 0;
    } else {
      use_zerofiller = 1;
    }

    at_langevin_integrator__init(langevin->integrator, mb, use_zerofiller);
  }

  /* the custom integration function must be set manually */
  langevin->integrate_func = NULL;

  return 0;
}


void at_langevin__clear(at_langevin_t *langevin)
{
  langevin->rejects = 0.0;
  langevin->total = 0.0;
}


void at_langevin__finish(at_langevin_t *langevin)
{
  at_langevin_integrator__finish(langevin->integrator);

  memset(langevin, 0, sizeof(*langevin));
}


#endif
