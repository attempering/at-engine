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


#include "at_mb__compare.h"


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


long ntimes = 100000;



static void init_mb_objects(at_utils_conf_t *conf,
    at_distr_t *distr, at_mb_t *mb1, at_mb_t *mb2)
{
  at_distr__conf_init_ez_(distr, conf);

  at_mb__conf_init_ez_(mb1, distr, conf);

  at_mb__conf_init_ez_(mb2, distr, conf);
}



static void mb_mock_sampling(at_mb_t *mb, long ntimes)
{
  long t;
  const uint32_t seed = 12345;
  zcom_rng_mt19937_t *rng = zcom_rng_mt19937__open(seed);
  at_distr_domain_t *domain = mb->distr->domain;

  for (t = 1; t <= ntimes; t++) {
    double beta = domain->beta_min + zcom_rng_mt19937__rand_01(rng) * (domain->beta_max - domain->beta_min);

    /* for the Gaussian energy model
     * Ec = - sigma^2 beta
     * and the energy fluctuation is sigma
     */
    double epot_c = -gaussian_sigma*gaussian_sigma * beta;
    double epot = epot_c + gaussian_sigma * zcom_rng_mt19937__rand_gauss(rng);
    int ib;

    at_mb__add(mb, epot, beta, &ib, NULL, NULL);

    if (t % 10000 == 0) {
      printf("%g%%%20s\r", 100.*t/ntimes, "");
    }
  }

  printf("%30s\n", "");

  zcom_rng_mt19937__close(rng);
}


static int test_io(at_mb_t *mb1, at_mb_t *mb2)
{
  at_bool_t passed;
  const char *fn = "mb.dat";
  int version2 = 0;

  at_mb__write_binary(mb1, fn);

  at_mb__read_binary(mb2, fn, &version2);

  if (at_mb__equal(mb1, mb2)) {
    passed = AT__TRUE;
  } else {
    passed = AT__FALSE;
  }

  return passed;
}


int main(int argc, char **argv)
{
  at_utils_conf_t conf[1];
  at_distr_t distr[1];
  at_mb_t mb1[1], mb2[1];

  at_bool_t passed;

  at_utils_conf__init_ez(conf, "at.cfg", "atdata", AT__FALSE);
  init_mb_objects(conf, distr, mb1, mb2);

  if (argc > 1) {
    ntimes = atol(argv[1]);
  }

  // manufacture moments data by sampling
  mb_mock_sampling(mb1, ntimes);

  // test integral-identity estimators with different window-division conventions
  passed = test_io(mb1, mb2);

  at_mb__finish(mb1);
  at_mb__finish(mb2);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
