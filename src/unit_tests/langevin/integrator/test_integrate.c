/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

/* testing the effect of the window boundary convention */


//#define AT_MB_DBG__ 1
#include "at/at__src.h"



double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


long ntimes = 100000;



void init_mb_and_langevin(at_mb_t *mb, at_langevin_t *langevin)
{
  zcom_cfg_t *cfg = zcom_cfg__open("at.cfg");

  // beta_min and beta_max are to be read from the configuration file
  at_mb__cfg_init(mb, cfg, boltz, NULL, NULL, 1);

  at_langevin__cfg_init(langevin, mb, cfg, NULL, NULL, 1);

  zcom_cfg__close(cfg);
}



double *mb_mock_sm_moments(at_mb_t *mb, double fill_prob)
{
  int i;
  at_mb_sm_t *sm;
  zcom_mtrng_t mtrng[1];
  double *data;

  zcom_mtrng__init_from_seed(mtrng, 12345);

  data = (double *) calloc(mb->n, sizeof(double));

  for (i = 0; i < mb->n; i++) {

    if (zcom_mtrng__rand01(mtrng) < fill_prob) {

      sm = at_mb_accum__get_proper_sums(mb->accum, i, i);
      double beta = mb->bmin + (i + 0.5) * mb->bdel;
      double epot = -beta * (gaussian_sigma * gaussian_sigma);

      sm->s = 1.0;
      sm->se = epot;

      data[i] = epot;

    } else {

      data[i] = 0.0;

    }

  }

  return data;
}



static double get_exact_integral(double beta1, double beta2)
{
  // E = -beta * sigma^2
  return -gaussian_sigma * gaussian_sigma * 0.5 * (beta2 * beta2 - beta1 * beta1);
}



static int test_integrate(at_mb_t *mb, at_langevin_t *langevin, double fill_prob)
{
  double *raw_data = mb_mock_sm_moments(mb, fill_prob);
  int passed;

  at_langevin_integrator_t intgr[1];
  at_langevin_zerofiller_t *zf = intgr->zerofiller;

  at_langevin_integrator__init(intgr, mb, 1);

  double beta1 = mb->bmin * 0.7 + mb->bmax * 0.3;
  double beta2 = mb->bmin * 0.2 + mb->bmax * 0.8;
  int i, ib1, ib2;

  double integral = at_langevin_integrator__integrate(intgr, beta1, beta2);
  double exact_integral = get_exact_integral(beta1, beta2);

  ib1 = intgr->ib_begin;
  ib2 = intgr->ib_end;

  at_langevin_zerofiller__fill_range_with_proper_sums(zf, ib1, ib2);

  for (i = ib1; i <= ib2; i++) {
    fprintf(stderr, "%d: %g %g\n", i, raw_data[i], zf->vals[i]);
  }

  fprintf(stderr, "integral %g vs %g\n", integral, exact_integral);

  at_langevin_integrator__finish(intgr);

  free(raw_data);

  passed = (fabs(integral - exact_integral) < 5.0);

  return passed;
}




int main(int argc, char **argv)
{
  at_mb_t mb[1];
  at_langevin_t langevin[1];
  double fill_prob = 0.5;
  int passed;

  init_mb_and_langevin(mb, langevin);

  if (argc > 1) {
    fill_prob = atof(argv[1]);
  }

  passed = test_integrate(mb, langevin, fill_prob);

  at_mb__finish(mb);
  at_langevin__finish(langevin);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
