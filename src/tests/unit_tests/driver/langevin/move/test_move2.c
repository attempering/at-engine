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

/**
 * Testing if the Langevin move can achieve
 * the flat beta distribution
 * on the Gaussian energy model
 *
 *
 */


//#define AT_MB_DBG__ 1
#include "at-engine__src.h"
#include "veritools/utilities/histogram/histogram.h"
#include "zcom/zcom__src.h"


double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


int nsteps = 100000;


int exact_beta_sampling = 0;


// using the exact partition function
// for the correction of the Langevin equation
double custom_integrate_func(at_mb_t *mb, double beta_old, double beta_new)
{
  return -0.5 * gaussian_sigma * gaussian_sigma * (beta_new * beta_new - beta_old * beta_old);
}



void init_distr_mb_langevin_objects(at_distr_t *distr, at_mb_t *mb, at_driver_langevin_t *langevin)
{
  zcom_cfg_t *cfg = zcom_cfg__open("at.cfg");
  at_bool_t verbose = 0;
  double boltz = 1.0;

  at_distr__cfg_init(distr, cfg, boltz, verbose);

  at_mb__cfg_init(mb, distr, cfg, boltz, NULL, NULL, verbose);

  at_driver_langevin__cfg_init(langevin, distr, mb, cfg, NULL, NULL, verbose);

  zcom_cfg__close(cfg);
}



void mb_mock_exact_moments(at_mb_t *mb)
{
  int i;
  at_mb_sm_t *sm;
  at_distr_domain_t *domain = mb->distr->domain;

  for (i = 0; i < domain->n; i++) {
    sm = at_mb_accum__get_proper_sums(mb->accum, i, i);
    double beta = domain->bmin + (i + 0.5) * domain->bdel;
    double epot = -beta * (gaussian_sigma * gaussian_sigma);

    sm->s = 1.0;
    sm->se = epot;
    sm->se2 = gaussian_sigma * gaussian_sigma;
    sm->se3 = 0;
  }

  mb->use_win_accum = AT__FALSE;
}



int test_langevin_move(at_mb_t *mb, at_driver_langevin_t *langevin,
    int nsteps)
{
  at_distr_domain_t *domain = mb->distr->domain;
  int step;
  double beta_c = (domain->bmin + domain->bmax) * 0.5;
  double beta = beta_c;
  double energy;
  int ib;
  double neg_dlnwf_dbeta = 0.0;
  double ergt;
  double invwf;
  zcom_mtrng_t mtrng[1];
  double vis_min = 1e10, vis_max = 0, flatness;
  int n = domain->n;
  int passed = 0;

  zcom_mtrng__init_from_seed(mtrng, 12345*time(NULL));

  fprintf(stderr, "beta %g (%g, %g)\n", beta, domain->bmin, domain->bmax);

  for (step = 0; step < nsteps; step++) {

    // exact configuration sampling at the current temperature
    energy = -beta * (gaussian_sigma * gaussian_sigma)
           + gaussian_sigma * zcom_mtrng__randgaus(mtrng);

    at_mb__add(mb, energy, beta, &ib, &invwf, &neg_dlnwf_dbeta);

    // use the Langevin equation to update the temperature
    beta = at_driver_langevin__move(langevin, mb, energy,
        beta, ib, invwf, neg_dlnwf_dbeta, &ergt);

    //fprintf(stderr, "step %d: beta %g, energy %g, invwf %g\n", step, beta, energy, invwf);
  }

  // histogram flatness u 1:9
  at_mb__write_ze_file(mb, "ze.dat");

  for (ib = 0; ib < n; ib++) {
    if (mb->visits[ib] < vis_min) {
      vis_min = mb->visits[ib];
    }
    if (mb->visits[ib] > vis_max) {
      vis_max = mb->visits[ib];
    }
  }

  flatness = (vis_max - vis_min) / (vis_max + vis_min);
  passed = (flatness < 0.3);
  fprintf(stderr, "flatness %g%%, passed %d\n", flatness*100, passed);

  return passed;
}



int main(int argc, char **argv)
{
  at_distr_t distr[1];
  at_mb_t mb[1];
  at_driver_langevin_t langevin[1];
  int passed;

  init_distr_mb_langevin_objects(distr, mb, langevin);

  // override the time step from the configuration file 
  if (argc > 1) {
    langevin->dt = atof(argv[1]);
    fprintf(stderr, "Langevin dt: %g\n", langevin->dt); //getchar();
  }

  // manufacture exact moments data
  mb_mock_exact_moments(mb);

  if (exact_beta_sampling) {
    langevin->integrate_func = custom_integrate_func;
  }

  passed = test_langevin_move(mb, langevin, nsteps);

  at_distr__finish(distr);
  at_mb__finish(mb);
  at_driver_langevin__finish(langevin);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
