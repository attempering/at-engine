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

#define GAUSSIAN__USE_MDSYS_ALIASES__
#include "veritools/models/gaussian-energy/gaussian.h"

#include "at-engine__src.h"

typedef long long at_llong_t;
double boltz = 1.0;
uint32_t langevin_seed = 1234;

double sigma = 100.0;     /* standard deviation of energy distributions */
double epot_dt = 1e-2;   /* equilibration rate, larger value means the system is able to equilibrate faster */
//double epot_dt = 1e-4; // use this to emulate a slow system
at_llong_t nsteps = 5000000;

void run_at_md(at_t *at, mdsys_t *mdsys, at_llong_t nsteps)
{
  at_llong_t step = 0;
  at_params_step_t step_params[1];
  double acc;

  zcom_rng_mt19937__init_from_seed(at->driver->langevin->rng->mtrng, langevin_seed);

  mdsys__set_energy_at_beta(mdsys, at->beta);

  for (step = 1; step <= nsteps; step++)
  {

    mdsys__step(mdsys, at->beta);

    if (at__do_tempering_on_step(at, step, AT__TRUE))
    {
      at->energy = mdsys->epot;

      step_params->step = step;
      step_params->is_first_step = (step == 1);
      step_params->is_last_step = (step == nsteps);
      step_params->do_trace = AT__FALSE;
      step_params->flush_output = AT__FALSE;

      //printf("step %ld, energy %g\n", (long) step, at->energy);
      at__move(at, step_params);

      // at_mb__write(at->mb, at->langevin);
      // exit(1);
    }
  }

  acc = at__get_move_acceptance_ratio(at);

  at_utils_log__info(at->log, "acceptance ratio: %g%%\n", acc*100.0);
}

int main(int argc, char **argv)
{
  const char *fn_cfg = "at.cfg";
  mdsys_t *mdsys;

  if (argc > 1)
  {
    fn_cfg = argv[1];
    fprintf(stderr, "reading configuration file %s\n", fn_cfg);
  }

  at_t *at = at__open(fn_cfg, NULL, AT__INIT_VERBOSE);

  at__manifest(at);

  mdsys = mdsys__new(sigma, epot_dt, at->distr->domain->beta_min, at->distr->domain->beta_max, boltz);

  run_at_md(at, mdsys, nsteps);

  at_utils_log__info(at->log, "AT simulation finished successfully.\n");

  mdsys__delete(mdsys);

  at__close(at);

  return 0;
}
