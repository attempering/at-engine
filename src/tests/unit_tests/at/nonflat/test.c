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

at_llong_t nsteps = 2000000;
double sigma = 100.0;


static double *get_expected_distr(at_t *at)
{
  zcom_cfg_t *cfg = at->utils->conf->cfg;
  double expo, beta0, sigma;
  at_distr_domain_t *domain = at->distr->domain;
  int i, n = domain->n;
  double w_tot;
  double *distr;

  zcom_cfg__get(cfg, &expo, "ensemble-factor", "%lf");
  zcom_cfg__get(cfg, &beta0, "ensemble-beta0", "%lf");
  zcom_cfg__get(cfg, &sigma, "ensemble-sigma", "%lf");

  distr = calloc(n, sizeof(double));

  w_tot = 0.0;
  for (i = 0; i < n; i++) {
    double beta = at_distr_domain__get_bin_center(domain, i);
    double w = 1.0/at_distr_weights__calc_inv_weight_bounded(
        at->distr->weights, beta, NULL, NULL, NULL);
    distr[i] = w;
    w_tot += w;
  }

  for (i = 0; i < n; i++) {
    distr[i] /= w_tot;
  }

  return distr;
}


static double *get_hist_distr(at_t *at)
{
  at_distr_domain_t *domain = at->distr->domain;
  int i, n = domain->n;
  double *distr;
  double w_tot;

  distr = calloc(n, sizeof(double));

  w_tot = 0.0;
  for (i = 0; i < n; i++) {
    distr[i] = at->mb->visits[i];
    w_tot += distr[i];
  }

  for (i = 0; i < n; i++) {
    distr[i] /= w_tot;
  }

  return distr;
}


static void test_distr(at_t *at)
{
  double *expected_distr = get_expected_distr(at);
  double *hist_distr = get_hist_distr(at);
  at_distr_domain_t *domain = at->distr->domain;
  int i, n = domain->n;
  double err, max_err = 0;
  double kl_div = 0.0;

  for (i = 0; i < n; i++) {
    err = fabs(hist_distr[i]/expected_distr[i] - 1);
    if (err > max_err) {
      max_err = err;
    }

    // compute the KL divergence
    if (hist_distr[i] > 0.0) {
      kl_div += hist_distr[i] * log(hist_distr[i]/expected_distr[i]);
    }

    double beta = at_distr_domain__get_bin_center(domain, i);

    fprintf(stderr, "%g %g %g\n", beta, expected_distr[i], hist_distr[i]);
  }

  kl_div *= at->distr->domain->beta_del;

  fprintf(stderr, "maximum relative distribution error %g%%, KL divergence %g\n\n", max_err*100, kl_div);

  if (kl_div < 1e-5) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  free(expected_distr);
  free(hist_distr);
}



static int work(int argc, char **argv)
{
  at_llong_t step = 0;
  double epot = 0.0;
  const char *fn_cfg = "at.cfg";
  zcom_rng_mt19937_t mtrng[1];
  at_params_step_t step_params[1];

  at_t* at = at__open(fn_cfg, NULL, AT__INIT_VERBOSE);

  (void) argc;
  (void) argv;

  at__manifest(at);

  zcom_rng_mt19937__init_from_seed(mtrng, 12345);

  epot = -sigma * sigma * at__get_beta(at)
         + sigma * zcom_rng_mt19937__rand_gauss(mtrng);

  //fprintf(stderr, "0 %g %g\n", epot, at__get_beta(at));

  for (step = 1; step <= nsteps; step++) {

    step_params->step = step;
    step_params->is_first_step = (step == 1);
    step_params->is_last_step = (step == nsteps);
    step_params->do_trace = AT__FALSE;
    step_params->flush_output = AT__FALSE;

    epot = -sigma * sigma * at__get_beta(at)
         + sigma * zcom_rng_mt19937__rand_gauss(mtrng);

    at__set_energy(at, epot);

    at__move(at, step_params);

    //if (step % 10000 == 0) {
    //  fprintf(stderr, "%ld %g %g\n", (long) step, epot, at__get_beta(at));
    //}
  }

  test_distr(at);

  at__close(at);

  return 0;
}


int main(int argc, char **argv)
{
  work(argc, argv);

  return 0;
}
