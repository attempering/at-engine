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


double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


long ntimes = 100000;



void init_mb_objects(at_distr_t *distr, at_mb_t *mb1, at_mb_t *mb2)
{
  zcom_cfg_t *cfg = zcom_cfg__open("at.cfg", ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__ALLOW_DASHES);
  at_bool_t verbose = 0;

  at_distr__cfg_init(distr, cfg, boltz, verbose);

  at_mb__cfg_init(mb1, distr, cfg, boltz, NULL, NULL, verbose);

  at_mb__cfg_init(mb2, distr, cfg, boltz, NULL, NULL, verbose);

  zcom_cfg__close(cfg);
}



void mb_mock_sampling(at_mb_t *mb, long ntimes)
{
  long t;
  const uint32_t seed = 12345;
  zcom_mtrng_t *rng = zcom_mtrng__open(seed);
  at_distr_domain_t *domain = mb->distr->domain;

  for (t = 1; t <= ntimes; t++) {
    double beta = domain->beta_min + zcom_mtrng__rand01(rng) * (domain->beta_max - domain->beta_min);

    /* for the Gaussian energy model
     * Ec = - sigma^2 beta
     * and the energy fluctuation is sigma
     */
    double epot_c = -gaussian_sigma*gaussian_sigma * beta;
    double epot = epot_c + gaussian_sigma * zcom_mtrng__randgaus(rng);
    int ib;

    at_mb__add(mb, epot, beta, &ib, NULL, NULL);

    if (t % 10000 == 0) {
      printf("%g%%%20s\r", 100.*t/ntimes, "");
    }
  }

  printf("%30s\n", "");

  zcom_mtrng__close(rng);
}


static int test_io(at_mb_t *mb1, at_mb_t *mb2)
{
  at_bool_t passed;
  const char *fn = "mb.dat";
  const int version = 1;
  int version2 = 0;

  at_mb__write_binary(mb1, fn, version);

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
  at_distr_t distr[1];
  at_mb_t mb1[1], mb2[1];

  int passed;

  init_mb_objects(distr, mb1, mb2);

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
