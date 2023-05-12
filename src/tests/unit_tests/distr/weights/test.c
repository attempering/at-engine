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


double calc_weight_ref(double expo, double beta0, double sigma, double bmax, double beta)
{
  double dx = (beta - beta0) / sigma;
  double w = exp(-0.5*dx*dx);
  double b_rel = beta/bmax;
  double f = pow(b_rel, -expo);

  return f * w;
}


void test_distr(zcom_cfg_t *cfg, at_distr_t *distr)
{
  at_distr_domain_t *domain = distr->domain;
  at_distr_weights_t *weights = distr->weights;
  int i;
  double beta, invwf, w_calc, w_ref;
  double expo, beta0, sigma;
  double err, max_err = 0.0;

  zcom_cfg__get(cfg, &expo, "ensemble_factor", "%lf");
  zcom_cfg__get(cfg, &beta0, "ensemble_beta0", "%lf");
  zcom_cfg__get(cfg, &sigma, "ensemble_sigma", "%lf");

  for (i = 0; i < domain->n; i++) {
    beta = domain->bmin + (i + 0.5) * domain->bdel;

    invwf = at_distr_weights__calc_inv_weight(weights, beta, NULL, NULL, NULL);
    w_calc = 1.0/invwf;

    w_ref = calc_weight_ref(expo, beta0, sigma, domain->bmax, beta);

    err = fabs(w_calc - w_ref);
    if (err > max_err) {
      max_err = err;
    }

    fprintf(stderr, "  %.4f: %.6f %.6f\n", beta, w_calc, w_ref);
  }

  fprintf(stderr, "maximum error: %g\n\n", max_err);

  if (max_err < 1e-6) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

}


int main(int argc, char **argv)
{
  at_distr_t distr[1];
  zcom_cfg_t *cfg = NULL;
  at_utils_manifest_t manifest[1];
  double boltz = 1.0;
  at_bool_t verbose = AT__TRUE;

  if (argc > 1) {
    cfg = zcom_cfg__open(argv[1]);
  }

  at_utils_manifest__cfg_init(manifest, cfg, NULL, NULL, verbose);
  at_utils_manifest__open_file(manifest);

  at_distr__cfg_init(distr, cfg, boltz, verbose);
  at_distr__manifest(distr, manifest);

  test_distr(cfg, distr);

  at_distr__finish(distr);

  at_utils_manifest__finish(manifest);

  if (cfg != NULL) {
    zcom_cfg__close(cfg);
  }

  return 0;
}