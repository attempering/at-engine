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
#include "at/at__src.h"



double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


long ntimes = 100000;



void init_mb_object(at_distr_t *distr, at_mb_t *mb)
{
  zcom_cfg_t *cfg = zcom_cfg__open("at.cfg");
  int verbose = 0;

  at_distr__cfg_init(distr, cfg, boltz, verbose);
  at_mb__cfg_init(mb, distr, cfg, boltz, NULL, NULL, verbose);

  zcom_cfg__close(cfg);
}



void mb_mock_exact_moments(at_mb_t *mb, double fill_prob)
{
  int i;
  zcom_mtrng_t mtrng[1];
  at_distr_domain_t *domain = mb->distr->domain;

  zcom_mtrng__init_from_seed(mtrng, time(NULL));

  for (i = 0; i < domain->n; i++) {
    at_mb_sm_t *sm = at_mb_accum__get_proper_sums(mb->accum, i, i);

    if (zcom_mtrng__rand01(mtrng) < fill_prob) {
      double beta = domain->bmin + (i + 0.5) * domain->bdel;
      double epot = -beta * (gaussian_sigma * gaussian_sigma);

      sm->s = 1.0;
      sm->se = epot;
      sm->se2 = gaussian_sigma * gaussian_sigma;
      sm->se3 = 0;
    } else {
      sm->s = 0;
      sm->se = 0;
      sm->se2 = 0;
      sm->se3 = 0;
    }

  }

  mb->flags &= ~MB_USE_WIN_ACCUM;
}



void mb_mock_sampling(at_mb_t *mb, long ntimes)
{
  long t;
  zcom_mtrng_t *rng = zcom_mtrng__open(time(NULL));
  at_distr_domain_t *domain = mb->distr->domain;

  for (t = 1; t <= ntimes; t++) {
    double beta = domain->bmin + zcom_mtrng__rand01(rng) * (domain->bmax - domain->bmin);

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


static int test_iie(at_mb_t *mb, double tol)
{
  at_distr_domain_t *domain = mb->distr->domain;
  int ib;
  int stride = (int) (domain->n / 5);
  int passed = 1;

  if (stride < 1) {
    stride = 1;
  }

  for (ib = 0; ib < domain->n; ib += stride) {

    fprintf(stderr, "\n# Testing integral-identity estimator for bin %d:\n", ib);

    // beta at the center of the bin
    double beta = domain->bmin + (ib + 0.5) * domain->bdel;

    // reference energy of the Gaussian energy model
    double et_ref = -beta * (gaussian_sigma * gaussian_sigma);

    // testing the left-right estimator with different
    // window-division conventions
    double et_legacy = at_mb_iie_et__calc_et_iie_lr(mb->iie, ib, MB_IIE_LR__WIN_DIV_LEGACY);
    double et_paper = at_mb_iie_et__calc_et_iie_lr(mb->iie, ib, MB_IIE_LR__WIN_DIV_PAPER);

    fprintf(stderr, "Et  %g (legacy convention)\n    %g (paper convention)\nRef %g\n\n",
      et_legacy, et_paper, et_ref);

    if (fabs(et_legacy - et_ref) > tol
      || fabs(et_paper - et_ref) > tol) {
      passed = 0;
    }

  }

  return passed;
}


int main(int argc, char **argv)
{
  at_distr_t distr[1];
  at_mb_t mb[1];

  int test_exact = 0;

  double tol;

  double fill_prob = 1.0;

  int passed;

  init_mb_object(distr, mb);

  if (argc > 1) {
    test_exact = (argv[1][0] == 'x');
  } else {
    printf("\nTesting exact moments? y/[n] ");
    char c = getchar();

    test_exact = (c == 'y' || c == 'Y');
  }



  if (test_exact) {

    if (argc > 2) {
      fill_prob = atof(argv[2]);
    }

    // manufacture moments data
    // with the exact moments data, both conventions
    // should yield exactly the same results
    mb_mock_exact_moments(mb, fill_prob);

    tol = 1.0;

  } else {

    if (argc > 2) {
      ntimes = atol(argv[2]);
    } else {

      char buf[100];

      printf("Enter the number of samples [%ld]: ", ntimes);

      if (fgets(buf, sizeof buf, stdin)) {
        long ntimes_user = 0;
        if (1 == sscanf(buf, "%ld", &ntimes_user)) {
          ntimes = ntimes_user;
        }
      }
    }

    // manufacture moments data by sampling
    mb_mock_sampling(mb, ntimes);

    tol = 5.0;

  }

  // test integral-identity estimators with different window-division conventions
  passed = test_iie(mb, tol);

  at_mb__finish(mb);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
