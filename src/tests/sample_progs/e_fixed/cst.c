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

#include "at-engine__src.h"



typedef long long at_llong_t;
double boltz = 1.0;
int use_minicst = 0;
uint32_t langevin_seed = 1234;

double sigma = 10.0; /* standard deviation of energy distributions */
double epot_dt = 0.01; /* equilibration rate, larger value means the system is able to equilibrate faster */
at_llong_t nsteps = 500000;

double epot = -100.0;


void run_cst_md(at_t* at, at_llong_t nsteps)
{
  at_llong_t step = 0;
  at_params_step_t step_params[1];

  zcom_mtrng__init_from_seed(at->driver->langevin->rng->mtrng, langevin_seed);

  for (step = 1; step <= nsteps; step++) {

    if (at__do_tempering_on_step(at, step, AT__TRUE))
    {
      step_params->step = step;
      step_params->is_first_step = (step == 1);
      step_params->is_last_step = (step == nsteps);
      step_params->flush_output = AT__FALSE;

      at->energy = epot;
      
      at__move(at, step_params);

    }

  }

}


int main(int argc, char** argv)
{
  const char* fn_cfg = "at.cfg";

  if (argc > 1) {
    fn_cfg = argv[1];
    fprintf(stderr, "reading configuration file %s\n", fn_cfg);
  }

  remove("atdata/trace.dat");

  at_t* at = at__open(fn_cfg, NULL, AT__INIT_VERBOSE);
  at__manifest(at);

  run_cst_md(at, nsteps);

  fprintf(stderr, "CST simulation finished successfully.\n");

  at__close(at);

  return 0;
}
