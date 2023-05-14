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
int use_minicst = 0;
uint32_t langevin_seed = 1234;

double sigma = 100.0;     /* standard deviation of energy distributions */
double epot_dt = 0.01;   /* equilibration rate, larger value means the system is able to equilibrate faster */
at_llong_t nsteps = 5000000;

void run_cst_md(at_t *at, mdsys_t *mdsys, at_llong_t nsteps)
{
  at_llong_t step = 0;
  at_params_step_t step_params[1];
  double acc;

  zcom_mtrng__init_from_seed(at->driver->langevin->rng->mtrng, langevin_seed);

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

      at__move(at, step_params);

      // at_mb__write(at->mb, at->langevin);
      // exit(1);
    }
  }

  acc = at__get_move_acceptance_ratio(at);

  fprintf(stderr, "Acceptance ratio: %g%%\n", acc*100.0);
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

  remove("atdata/trace.dat");

  at_t *at = at__open(fn_cfg, NULL, 0);
  // at__manifest(at);

  mdsys = mdsys__new(sigma, epot_dt, at->distr->domain->beta_min, at->distr->domain->beta_max, boltz);

  run_cst_md(at, mdsys, nsteps);

  fprintf(stderr, "CST simulation finished successfully.\n");

  mdsys__delete(mdsys);

  at__close(at);

  return 0;
}
