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
#include "../../../zcom/zcom.h"



/* check if domain->barr is arranged in an ascending order */
static int at_distr_domain__check_barr(at_distr_domain_t *domain)
{
  int i;

  for (i = 0; i <= domain->n; i++)
    if (i > 0 && domain->barr[i] <= domain->barr[i-1]) {
      fprintf(stderr, "barr should ascend: barr[%d] = %g, barr[%d] = %g\n",
          i, domain->barr[i], i-1, domain->barr[i-1]);
      return 1;
    }
  return 0;
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

  domain->barr = calloc(n+1, sizeof(double));

  for (i = 0; i <= n; i++) {
    domain->barr[i] = beta_min + beta_del * i;
  }
}



int at_distr_domain__cfg_init(at_distr_domain_t *domain,
    zcom_cfg_t *cfg, at_bool_t verbose)
{
  int i;

  /* read beta_min and beta_max from the configuration file */

  domain->beta_min = 0.2;
  if (0 != zcom_cfg__get(cfg, &domain->beta_min, "beta_min", "%lf")) {
    fprintf(stderr, "Warning: missing var: distr->domain->beta_min, key: beta_min, fmt: %%lf, assuming %g\n", domain->beta_min);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
  }

  if ( !(domain->beta_min >= 0.0) ) {
    fprintf(stderr, "domain->beta_min: failed validation: distr->domain->beta_min %g > 1e-6\n",
        domain->beta_min);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }


  domain->beta_max = 0.4;
  if (0 != zcom_cfg__get(cfg, &domain->beta_max, "beta_max", "%lf")) {
    fprintf(stderr, "Warning: missing var: distr->domain->beta_max, key: beta_max, fmt: %%lf, assuming %g\n", domain->beta_max);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
  }

  //printf("domain->beta_min %g, domain->beta_max %g\n", domain->beta_min, domain->beta_max); getchar();

  if ( !(domain->beta_max >= domain->beta_min) ) {
    fprintf(stderr, "distr->domain->beta_max: failed validation: domain->beta_max %g > domain->beta_min %g\n",
        domain->beta_max, domain->beta_min);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* beta_del: bin size of beta */
  domain->beta_del = 0.005;

  if (0 != zcom_cfg__get(cfg, &domain->beta_del, "beta_del", "%lf")) {
    fprintf(stderr, "Warning: missing var: distr->domain->beta_del, key: beta_del, fmt: %%lf, assuming %g\n",
        domain->beta_del);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
  }

  if ( !(domain->beta_del > 1e-6) ) {
    fprintf(stderr, "distr->domain->beta_del: failed validation: domain->beta_del > 1e-6\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* n: number of temperature bins */
  domain->n = (int)((domain->beta_max - domain->beta_min)/domain->beta_del - 1e-5) + 1;

  //fprintf(stderr, "domain->n %d\n", domain->n);
  //getchar();

  /* barr: temperature array */
  if ((domain->barr = (double *) calloc((domain->n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "no memory! var: distr->domain->barr, type: double\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < domain->n+1; i++) {
    domain->barr[i] = domain->beta_min + i * domain->beta_del;
  }

  /* check beta array */
  if ( !(at_distr_domain__check_barr(domain) == 0) ) {
    fprintf(stderr, "check beta array\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  /* fix beta_max to a bin boundary */
  domain->beta_max = domain->beta_min + domain->beta_del * domain->n;

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
  FILE *fp = manifest->fp;

  //fprintf(stderr, "fp %p\n", manifest->fp); exit(1);

  /* beta_del: bin size of beta */
  fprintf(fp, "distr->domain->beta_del: double, %g\n", domain->beta_del);

  /* n: number of temperature bins */
  fprintf(fp, "distr->domain->n: int, %4d\n", domain->n);

  /* barr: temperature array */
  at_utils_manifest__print_double_arr(manifest, domain->barr, domain->n+1, "distr->domain->barr");
}



int at_distr_domain__beta_to_index(const at_distr_domain_t *domain, double beta, at_bool_t check)
{
  int j;

  if (beta >= domain->beta_min) {
    j = (int)((beta - domain->beta_min)/domain->beta_min);
  } else {
    j = -1;
  }

  if (check) {
    zcom_util__exit_if (j < 0 || j >= domain->n, "beta = %d, %g, range = (%g, %g, %g)\n",
        j, beta, domain->beta_min, domain->beta_del, domain->beta_max);
  }

  return j;
}




#endif
