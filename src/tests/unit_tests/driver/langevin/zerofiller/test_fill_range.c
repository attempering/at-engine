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

/* testing the effect of the window boundary convention */


//#define AT_MB_DBG__ 1
#include "at-engine__src.h"



double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


long ntimes = 100000;



void init_distr_mb_and_langevin(at_distr_t *distr, at_mb_t *mb, at_driver_langevin_t *langevin)
{
  zcom_cfg_t *cfg = zcom_cfg__open("at.cfg");
  at_bool_t verbose = 0;
  double boltz = 1.0;

  at_distr__cfg_init(distr, cfg, boltz, verbose);

  // beta_min and beta_max are to be read from the configuration file
  at_mb__cfg_init(mb, distr, cfg, boltz, NULL, NULL, verbose);

  at_driver_langevin__cfg_init(langevin, mb->distr, mb, cfg, NULL, NULL, verbose);

  zcom_cfg__close(cfg);
}



double *mb_mock_sm_moments(at_mb_t *mb, double fill_prob)
{
  int i;
  at_distr_domain_t *domain = mb->distr->domain;
  at_mb_sm_t *sm;
  zcom_mtrng_t mtrng[1];
  double *data;

  zcom_mtrng__init_from_seed(mtrng, 12345);

  data = (double *) calloc(domain->n, sizeof(double));

  for (i = 0; i < domain->n; i++) {

    if (zcom_mtrng__rand01(mtrng) < fill_prob) {

      sm = at_mb_accum__get_proper_sums(mb->accum, i, i);
      double beta = domain->beta_min + (i + 0.5) * domain->beta_del;
      double epot = -beta * (gaussian_sigma * gaussian_sigma);

      sm->s = 1.0;
      sm->se = epot;

      data[i] = epot;

    } else {

      data[i] = 0.0;

    }

    //fprintf(stderr, "%d %g\n", i, data[i]);

  }

  return data;
}



static int test_fill_range(at_mb_t *mb, at_driver_langevin_t *langevin)
{
  double *raw_data = mb_mock_sm_moments(mb, 0.2);

  at_driver_langevin_zerofiller_t zf[1];

  int i;

  int passed = 1;
  int n = mb->distr->domain->n;

  at_driver_langevin_zerofiller__init(zf, n);

  at_driver_langevin_zerofiller__fill_range_with_proper_sums(zf, 0, n - 1, mb);

  for (i = 0; i < n; i++) {
    fprintf(stderr, "%d: %g %g\n", i, raw_data[i], zf->vals[i]);
    if (zf->vals[i] == 0.0) {
      passed = 0;
    }
  }

  at_driver_langevin_zerofiller__finish(zf);

  free(raw_data);

  return passed;
}




int main(void)
{
  at_distr_t distr[1];
  at_mb_t mb[1];
  at_driver_langevin_t langevin[1];
  int passed;

  init_distr_mb_and_langevin(distr, mb, langevin);

  passed = test_fill_range(mb, langevin);

  at_distr__finish(mb->distr);
  at_mb__finish(mb);
  at_driver_langevin__finish(langevin);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
