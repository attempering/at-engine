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


void test_distr(zcom_cfg_t *cfg, at_distr_t *distr)
{
  at_distr_domain_t *domain = distr->domain;
  at_distr_weights_t *weights = distr->weights;
  int i, n = domain->n;
  double beta, invwf, neg_dlnwf_dbeta, log_deriv;
  double *wf;
  const double tol = 10.0 * domain->beta_del;
  at_bool_t passed = AT__TRUE;

  wf = (double *) calloc(n + 1, sizeof(double));

  for (i = 0; i <= n; i++) {
    beta = domain->beta_min + i * domain->beta_del;
    invwf = at_distr_weights__calc_inv_weight(weights, beta, NULL, NULL, NULL);
    wf[i] = 1.0/invwf;
  }

  for (i = 0; i < n; i++) {
    beta = at_distr_domain__get_bin_center(domain, i);

    at_distr_weights__calc_inv_weight(weights, beta, &neg_dlnwf_dbeta, NULL, NULL);

    log_deriv = log(wf[i]/wf[i+1])/domain->beta_del;

    if (fabs(log_deriv - neg_dlnwf_dbeta) > tol) {
      printf("%.4f %.6f %.6f\n", beta, neg_dlnwf_dbeta, log_deriv);
      passed = AT__FALSE;
    } 

    //printf("%.4f %.6f %.6f\n", beta, neg_dlnwf_dbeta, log_deriv);
  }

  free(wf);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

}


int main(int argc, char **argv)
{
  at_distr_t distr[1];
  const char *cfg_fn = "ensemble-mode-3.cfg";
  zcom_cfg_t *cfg = NULL;
  at_utils_manifest_t manifest[1];
  double boltz = 1.0;
  at_bool_t verbose = AT__TRUE;

  if (argc > 1) {
    cfg_fn = argv[1];
  }

  cfg = zcom_cfg__open(cfg_fn, ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__ALLOW_DASHES);

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
