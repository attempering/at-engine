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

#include "veritools/models/lj/lj.h"
#include "veritools/models/lj/ljeos.h"

#include "at-engine__src.h"


double boltz = 1.0;

int n = 108;
at_llong_t nequil = 5000;
at_llong_t nsteps = 200000;
double rho = 0.7;
double rc_def = 2.5;
double md_dt = 0.002;
double thermostat_dt = 0.02;



static void equilibrate(lj_t *lj, double temp)
{
  at_llong_t step;

  /* equilibration */
  for (step = 1; step <= nequil; step++) {
    lj__vv(lj, md_dt);
    lj__vel_rescale(lj, temp, thermostat_dt);
    if (step % 1000 == 0) {
      printf(at_llong_pfmt ", U %g, P %g\n",
          step, lj->epot,
          lj__calc_pres(lj, temp));
    }
  }
}



static void run_at_md(at_t* at, lj_t* lj, at_llong_t nsteps)
{
  at_llong_t step = 0;
  at_params_step_t step_params[1];
  double temp_init = at->utils->thermostat_temp;
  double acc;

  at_params_step__init(step_params);

  at__set_beta(at, at__temp_to_beta(at, temp_init));

  for (step = 1; step <= nsteps; step++) {

    lj__vvx(lj, md_dt, at->force_scale);
    lj__vel_rescale(lj, at->utils->thermostat_temp, thermostat_dt);

    if (at__do_tempering_on_step(at, step, AT__TRUE)) {
      at->energy = lj->epot;

      step_params->step = step;
      step_params->is_first_step = (step == 1);
      step_params->is_final_step = (step == nsteps);
      step_params->do_trace = AT__FALSE;
      step_params->flush_output = AT__FALSE;

      at__move(at, step_params);
    }
  }

  acc = at__get_move_acc_ratio(at);

  at_utils_logger__info(at->logger, "acceptance ratio: %g%%\n", acc*100.0);

  at_utils_logger__info(at->logger, "AT simulation finished successfully.\n");

}


static int write_ref(at_t* at)
{
  const char* fn_ref = "ljeos.dat";
  FILE* fp;
  int i;

  if ((fp = fopen(fn_ref, "w")) == NULL) {
    fprintf(stderr, "failed to open %s\n", fn_ref);
    return -1;
  }

  for (i = 0; i < at->distr->domain->n; i++) {
    double beta = at->distr->domain->beta_min + (i + 0.5) * at->distr->domain->beta_del;
    double temp = at__beta_to_temp(at, beta);
    double pres;

    double epot = ljeos3d__get(rho, temp, &pres, NULL, NULL);

    fprintf(fp, "%g %g %g\n", beta, epot * n, pres);
  }

  fclose(fp);

  return 0;
}


int main(int argc, char** argv)
{
  const char *fn_cfg = "at.cfg";
  lj_t *lj;

  if (argc > 1)
  {
    fn_cfg = argv[1];
    fprintf(stderr, "reading configuration file %s\n", fn_cfg);
  }

  at_t *at = at__open(fn_cfg, NULL, AT__INIT_VERBOSE);

  at__manifest(at);

  lj = lj__open(n, rho, rc_def);

  equilibrate(lj, at->utils->thermostat_temp);

  run_at_md(at, lj, nsteps);

  write_ref(at);

  lj__close(lj);

  at__close(at);

  return 0;
}

