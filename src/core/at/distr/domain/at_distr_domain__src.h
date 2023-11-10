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

#ifndef AT_DISTR_DOMAIN__SRC_H__
#define AT_DISTR_DOMAIN__SRC_H__

#include "at_distr_domain.h"



/* check if domain->barr is arranged in an ascending order */
static void at_distr_domain__check_barr(at_distr_domain_t *domain,
    at_utils_logger_t *logger)
{
  int i;

  for (i = 0; i <= domain->n; i++) {
    if (i > 0 && domain->barr[i] <= domain->barr[i-1]) {
      at_utils_logger__fatal(logger,
          "beta array has incorrect ordering: barr[%d] = %g, barr[%d] = %g\n",
          i, domain->barr[i], i-1, domain->barr[i-1]);
    }
  }

}



void at_distr_domain__init_simple(at_distr_domain_t *domain,
    double beta_min, double beta_max, double beta_del)
{
  int i, n;

  n = (int) ((beta_max - beta_min)/beta_del + 0.5);
  domain->n = n;
  domain->beta_min = beta_min;
  domain->beta_max = beta_max;
  domain->beta_del = beta_del;

  if (domain->barr != NULL) {
    free(domain->barr);
  }

  at_utils__new_arr(domain->barr, n+1, double);

  for (i = 0; i <= n; i++) {
    domain->barr[i] = beta_min + beta_del * i;
  }
}



int at_distr_domain__conf_init(at_distr_domain_t *domain,
    at_utils_conf_t *conf)
{
  int i;

  at_utils_conf__push_mod(conf, "at.distr.domain");

  at_utils_conf__get_double(conf,
      "beta-min",
      &domain->beta_min, 0.2,
      "beta_min");

  if (domain->beta_min < 0.0) {
    at_utils_logger__error(conf->logger,
        "domain->beta_min: failed validation: distr->domain->beta_min %g > 1e-6\n",
        domain->beta_min);
    goto ERR;
  }

  at_utils_conf__get_double(conf,
      "beta-max",
      &domain->beta_max, 0.4,
      "beta_max");

  //printf("domain->beta_min %g, domain->beta_max %g\n", domain->beta_min, domain->beta_max); getchar();

  if (domain->beta_max < domain->beta_min) {
    at_utils_logger__error(conf->logger,
        "distr->domain->beta_max: failed validation: domain->beta_max %g > domain->beta_min %g\n",
        domain->beta_max, domain->beta_min);
    goto ERR;
  }

  /* beta_del: bin size of beta */
  at_utils_conf__get_double(conf,
      "beta-del",
      &domain->beta_del, 0.005,
      "beta_del");

  if (domain->beta_del < 1e-6) {
    at_utils_logger__error(conf->logger,
        "distr->domain->beta_del: failed validation: domain->beta_del > 1e-6\n");
    goto ERR;
  }

  /* n: number of temperature bins */
  domain->n = (int)((domain->beta_max - domain->beta_min)/domain->beta_del - 1e-5) + 1;

  //fprintf(stderr, "domain->n %d\n", domain->n);
  //getchar();

  /* barr: temperature beta array */
  at_utils__new_arr(domain->barr, domain->n+1, double);

  for (i = 0; i < domain->n+1; i++) {
    domain->barr[i] = domain->beta_min + i * domain->beta_del;
  }

  /* check beta array */
  at_distr_domain__check_barr(domain, conf->logger);

  /* fix beta_max to a bin boundary */
  domain->beta_max = domain->beta_min + domain->beta_del * domain->n;

  at_utils_conf__pop_mod(conf);

  return 0;

ERR:

  return -1;
}



void at_distr_domain__finish(at_distr_domain_t *domain)
{
  //fprintf(stderr, "barr %p %s:%d\n", domain->barr, __FILE__, __LINE__);
  if (domain->barr != NULL) {
    free(domain->barr);
    domain->barr = NULL;
  }
}

void at_distr_domain__manifest(const at_distr_domain_t *domain, at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.distr.domain");

  at_utils_manifest__print_double(manifest, domain->beta_min, "beta_min", "beta-min");

  at_utils_manifest__print_double(manifest, domain->beta_max, "beta_max", "beta-max");

  at_utils_manifest__print_double(manifest, domain->beta_del, "beta_del", "beta-del");

  at_utils_manifest__print_int(manifest, domain->n, "n", NULL);

  at_utils_manifest__print_double_arr(manifest, domain->barr, domain->n+1, "barr");

  at_utils_manifest__pop_mod(manifest);
}



int at_distr_domain__beta_to_index(
    const at_distr_domain_t *domain, double beta, at_bool_t check)
{
  int j;

  if (beta >= domain->beta_min) {
    j = (int)((beta - domain->beta_min)/domain->beta_del);
  } else {
    j = -1;
  }

  if (check) {
    zcom_utils__exit_if (j < 0 || j >= domain->n, "beta = %d, %g, range = (%g, %g, %g)\n",
        j, beta, domain->beta_min, domain->beta_del, domain->beta_max);
  }

  return j;
}



double at_distr_domain__get_bin_center(const at_distr_domain_t *domain, int ib)
{
  return domain->beta_min + (ib + 0.5) * domain->beta_del;
}



#endif
