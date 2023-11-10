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

#include "at/distr/at_distr__src.h"
#include "at/utils/at_utils__src.h"
#include "zcom/zcom__src.h"


static double calc_weight_ref(double expo, double beta0, double sigma, double beta_max, double beta)
{
  double dx = (beta - beta0) / sigma;
  double w = exp(-0.5*dx*dx);
  double b_rel = beta/beta_max;
  double f = pow(b_rel, -expo);
  //fprintf(stderr, "beta %g, beta0 %g, sigma %g, w %g, f %g\n", beta, beta0, sigma, w, f);

  return f * w;
}


static void test_distr(zcom_cfg_t *cfg, at_distr_t *distr)
{
  at_distr_domain_t *domain = distr->domain;
  at_distr_weights_t *weights = distr->weights;
  int i;
  double beta, w_ref;
  double invwf, w_calc;
  double invwf_simple, w_calc_simple;
  double lnwf, w_calc_ln;
  double expo, beta0, sigma;
  double err, max_err = 0;

  zcom_cfg__get(cfg, &expo, "ensemble-factor", "%lf");
  zcom_cfg__get(cfg, &beta0, "ensemble-beta0", "%lf");
  zcom_cfg__get(cfg, &sigma, "ensemble-sigma", "%lf");

  for (i = 0; i < domain->n; i++) {
    beta = at_distr_domain__get_bin_center(domain, i);

    invwf = at_distr_weights__calc_inv_weight_bounded(
        weights, beta, NULL, NULL, NULL);
    w_calc = 1.0/invwf;

    invwf_simple = at_distr_weights__calc_inv_weight_simple(
        weights, beta, NULL, NULL, NULL);
    w_calc_simple = 1.0/invwf_simple;

    lnwf = at_distr_weights__calc_lnwf(
        weights, beta);
    w_calc_ln = exp(lnwf);

    w_ref = calc_weight_ref(expo, beta0, sigma, domain->beta_max, beta);

    err = fabs(w_calc - w_ref);
    if (err > max_err) {
      max_err = err;
    }

    fprintf(stderr, "  %.4f: %.6g (bounded) %.6g (simple) %.6g (ln) %.6g (ref)\n",
        beta, w_calc, w_calc_simple, w_calc_ln, w_ref);
  }

  fprintf(stderr, "maximum error: %g\n\n", max_err);

  double tol = fmax(2.0*weights->f_min, 2.0/weights->invwf_max);

  if (max_err < tol) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

}


int main(int argc, char **argv)
{
  const char *cfg_fn = "ensemble-mode-1.cfg";
  at_utils_conf_t conf[1];
  at_utils_manifest_t manifest[1];
  at_distr_t distr[1];
  at_bool_t verbose = AT__TRUE;

  if (argc > 1) {
    cfg_fn = argv[1];
  }

  at_utils_conf__init_ez(conf, cfg_fn, "atdata", verbose);

  at_utils_manifest__conf_init(manifest, conf);
  at_utils_manifest__open_file(manifest);

  at_distr__conf_init_ez_(distr, conf);
  at_distr__manifest(distr, manifest);

  test_distr(conf->cfg, distr);

  at_distr__finish(distr);

  at_utils_manifest__finish(manifest);
  at_utils_conf__finish_ez(conf);

  return 0;
}