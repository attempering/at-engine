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



static void init_distr_mb_and_langevin(at_utils_conf_t *conf,
    at_distr_t *distr, at_mb_t *mb, at_driver_langevin_t *langevin)
{
  at_distr__conf_init_ez_(distr, conf);

  // beta_min and beta_max are to be read from the configuration file
  at_mb__conf_init_ez_(mb, distr, conf);

  at_driver_langevin__conf_init(langevin, mb->distr, mb, conf, 0);
}



static double *mb_mock_sm_moments(at_mb_t *mb, double fill_prob)
{
  int i;
  at_distr_domain_t *domain = mb->distr->domain;
  at_mb_sm_t *sm;
  zcom_rng_mt19937_t mtrng[1];
  double *data;

  zcom_rng_mt19937__init_from_seed(mtrng, 12345);

  data = (double *) calloc(domain->n, sizeof(double));

  for (i = 0; i < domain->n; i++) {

    if (zcom_rng_mt19937__rand_01(mtrng) < fill_prob) {

      sm = at_mb_accum__get_proper_sums(mb->accum, i, i);
      double beta = at_distr_domain__get_bin_center(domain, i);
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

  at_bool_t passed = AT__TRUE;
  int n = mb->distr->domain->n;

  (void) langevin;

  at_driver_langevin_zerofiller__init(zf, n);

  at_driver_langevin_zerofiller__fill_range_with_proper_sums(zf, 0, n - 1, mb);

  for (i = 0; i < n; i++) {
    fprintf(stderr, "%d: %g %g\n", i, raw_data[i], zf->vals[i]);
    if (zf->vals[i] == 0.0) {
      fprintf(stderr, "^ failure case\n");
      passed = AT__FALSE;
    }
  }

  at_driver_langevin_zerofiller__finish(zf);

  free(raw_data);

  return passed;
}




int main(void)
{
  at_utils_conf_t conf[1];
  at_bool_t verbose = AT__TRUE;
  at_distr_t distr[1];
  at_mb_t mb[1];
  at_driver_langevin_t langevin[1];
  at_bool_t passed;

  at_utils_conf__init_ez(conf, "at.cfg", "atdata", verbose);
  init_distr_mb_and_langevin(conf, distr, mb, langevin);

  passed = test_fill_range(mb, langevin);

  at_distr__finish(mb->distr);
  at_mb__finish(mb);
  at_driver_langevin__finish(langevin);
  at_utils_conf__finish_ez(conf);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
