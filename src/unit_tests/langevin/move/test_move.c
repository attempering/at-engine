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

/**
 * Testing the effect of the Metropolisation correction
 * to the Langevin move on the Gaussian energy model
 * under a fixed energy.
 * 
 * In this case, the beta distribution is expected to
 * recover an exact Gaussian.
 *
 */


/*

Use the following plotting command

plot "beta_hist_0.dat" u 1:3 w l, "beta_hist_1.dat" u 1:3 w l, exp(-((x-0.3)/0.01)**2/2)/(sqrt(2*pi)*0.01)

*/

//#define AT_MB_DBG__ 1
#include "at/at__src.h"
#include "veritools/utilities/histogram/histogram.h"


double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


int nsteps = 100000;


void init_mb_langevin_objects(at_mb_t *mb, at_langevin_t *langevin)
{
  zcom_cfg_t *cfg = zcom_cfg__open("at.cfg");
  int silent = 1;

  at_mb__cfg_init(mb, cfg, boltz, NULL, NULL, silent);

  at_langevin__cfg_init(langevin, mb, cfg, NULL, NULL, silent);

  zcom_cfg__close(cfg);
}



void mb_mock_exact_moments(at_mb_t *mb)
{
  int i;
  at_mb_sm_t *sm;

  // use the plain sums for simplicity
  mb->flags &= ~MB_USE_WIN_ACCUM;
  mb->accum->use_winaccum = 0;

  for (i = 0; i < mb->n; i++) {
    sm = at_mb_accum__get_proper_sums(mb->accum, i, i);
    double beta = mb->bmin + (i + 0.5) * mb->bdel;
    double epot = -beta * (gaussian_sigma * gaussian_sigma);

    sm->s = 1.0;
    sm->se = epot;
    sm->se2 = gaussian_sigma * gaussian_sigma;
    sm->se3 = 0;
  }

}



int test_langevin_move_no_cfg_sampling(at_mb_t *mb, at_langevin_t *langevin,
    int corrected, int nsteps)
{
  int step;
  double beta_c = (mb->bmin + mb->bmax) * 0.5;
  double beta = beta_c;
  double erg = -beta * (gaussian_sigma * gaussian_sigma);
  double beta_sigma = 1.0 / gaussian_sigma;
  double hist_beta_min, hist_beta_max;
  int ib;
  double neg_dlnwf_dbeta = 0.0;
  double ergt;
  double invwf;
  zcom_mtrng_t mtrng[1];
  histogram_t hist[1];
  char fn_hist[FILENAME_MAX];
  at_mb_sm_t sm_beta[1];

  int passed = 0;

  zcom_mtrng__init_from_seed(mtrng, 12345*time(NULL));

  hist_beta_min = fmax(mb->bmin, beta - 5*beta_sigma);
  hist_beta_max = fmin(mb->bmax, beta + 5*beta_sigma);

  histogram__init(hist, hist_beta_min, hist_beta_max, mb->bdel*0.1);

  fprintf(stderr, "beta %g (%g, %g)\n", beta, mb->bmin, mb->bmax);

  at_mb_sm__init(sm_beta);

  // disabling restriction on minimum visits
  // and stride moderation
  // as we have already the exact moments
  langevin->bin_min_visits = 0;
  langevin->no_skip = 0;

  // uncomment the next two lines to print diagnostic information
  //at_langevin_move__debug__ = 2;
  //at_mb_iie_et__debug__ = 1;

  int nst_debug_print = 0;
  if (at_langevin_move__debug__ || at_mb_iie_et__debug__) {
    nst_debug_print = 1;
  }

  // mimic the MD simulation loop
  // with a floating beta and fixed energy
  for (step = 0; step < nsteps; step++) {

    // calculate ib and invwf
    ib = at_mb__beta_to_index(mb, beta, 1);

    invwf = at_mb_betadist__calc_inv_weight(mb->betadist,
        beta, &neg_dlnwf_dbeta, NULL, NULL);

    // The pure beta distribution of the Gaussian model
    // as given by
    //
    //  exp(-beta*U)/Z(beta)
    //
    // is a strict Gaussian.
    //
    // If there is an extra ensemble factor wf,
    // we have to deduct each visit by a factor of 1.0/wf
    // in order to recover the native Gaussian.
    //
    histogram__wadd(hist, beta, invwf);
    at_mb_sm__add(sm_beta, invwf, beta, 0);

    // use the Langevin equation to update the temperature
    if (corrected) {
      beta = at_langevin__move_corrected(langevin, mb, erg,
          beta, ib, invwf, neg_dlnwf_dbeta, &ergt);
    } else {
      beta = at_langevin__move_simple(langevin, mb, erg,
          beta, ib, invwf, neg_dlnwf_dbeta, &ergt);
    }

    if (corrected && nst_debug_print > 0) {
      fprintf(stderr, "step %d: beta %g, ib %d, invwf %g\n", step, beta, ib, invwf);
      if (nst_debug_print > 0 && (step+1) % nst_debug_print == 0) getchar();
    }
  }


  {
    double beta_mean = at_mb_sm__get_mean(sm_beta, 0);
    double beta_std = at_mb_sm__get_std(sm_beta, 0);

    fprintf(stderr, "corrected [%d], beta: %g (%g, %g%%) +/- %g (%g, %g%%)\n",
        corrected, beta_mean, beta_c, (beta_mean/beta_c-1)*100,
        beta_std, beta_sigma, (beta_std/beta_sigma-1)*100);

    if (fabs(beta_std/beta_sigma - 1) < 0.02
     && fabs(beta_mean/beta_c - 1) < 0.001) {
      passed = 1;
    } else {
      passed = 0;
    }
  }


  if (corrected) {
    sprintf(fn_hist, "beta_hist_corrected.dat");
  } else {
    sprintf(fn_hist, "beta_hist_uncorrected.dat");
  }

  histogram__save(hist, fn_hist);
  histogram__finish(hist);

  fprintf(stderr, "Result: %d\n\n", passed);

  return passed;
}



int main(int argc, char **argv)
{
  at_mb_t mb[1];
  at_langevin_t langevin[1];
  int langevin_corrected;
  int passed;

  init_mb_langevin_objects(mb, langevin);

  // override the time step from the configuration file 
  if (argc > 1) {
    langevin->dt = atof(argv[1]);
    fprintf(stderr, "Langevin dt: %g\n", langevin->dt); //getchar();
  }

  // manufacture exact moments data
  mb_mock_exact_moments(mb);

  // test the uncorrected Langevin equation
  //langevin_corrected = 0;
  //fprintf(stderr, "Testing Langevin move dt=%g without correction:\n", langevin->dt);
  //test_langevin_move_no_cfg_sampling(mb, langevin, langevin_corrected, nsteps);

  // test the corrected Langevin equation
  langevin_corrected = 1;
  fprintf(stderr, "Testing Langevin move dt=%g with correction:\n", langevin->dt);
  passed = test_langevin_move_no_cfg_sampling(mb, langevin, langevin_corrected, nsteps);

  at_mb__finish(mb);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
