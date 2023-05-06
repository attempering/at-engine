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
    double bmin, double bmax, double bdel)
{
  int i, n;

  n = (int) ((bmax - bmin)/bdel + 0.5);
  domain->n = n;
  domain->bmin = bmin;
  domain->bmax = bmax;
  domain->bdel = bdel;
  domain->barr = calloc(n+1, sizeof(double));

  for (i = 0; i <= n; i++) {
    domain->barr[i] = bmin + bdel * i;
  }
}



int at_distr_domain__cfg_init(at_distr_domain_t *domain,
    zcom_cfg_t *cfg, at_bool_t verbose)
{
  int i;

  /* read bmin and bmax from the configuration file */

  domain->bmin = 0.2;
  if (0 != zcom_cfg__get(cfg, &domain->bmin, "beta_min", "%lf")) {
    fprintf(stderr, "Warning: missing var: distr->domain->bmin, key: beta_min, fmt: %%lf, assuming %g\n", domain->bmin);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
  }

  if ( !(domain->bmin >= 0.0) ) {
    fprintf(stderr, "domain->bmin: failed validation: distr->domain->bmin %g > 1e-6\n",
        domain->bmin);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }


  domain->bmax = 0.4;
  if (0 != zcom_cfg__get(cfg, &domain->bmax, "beta_max", "%lf")) {
    fprintf(stderr, "Warning: missing var: distr->domain->bmax, key: beta_max, fmt: %%lf, assuming %g\n", domain->bmax);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
  }

  //printf("domain->bmin %g, domain->bmax %g\n", domain->bmin, domain->bmax); getchar();

  if ( !(domain->bmax >= domain->bmin) ) {
    fprintf(stderr, "distr->domain->bmax: failed validation: domain->bmax %g > domain->bmin %g\n",
        domain->bmax, domain->bmin);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* bdel: bin size of beta */
  domain->bdel = 0.005;

  if (0 != zcom_cfg__get(cfg, &domain->bdel, "beta_del", "%lf")) {
    fprintf(stderr, "Warning: missing var: distr->domain->bdel, key: beta_del, fmt: %%lf, assuming %g\n",
        domain->bdel);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
  }

  if ( !(domain->bdel > 1e-6) ) {
    fprintf(stderr, "distr->domain->bdel: failed validation: domain->bdel > 1e-6\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* n: number of temperature bins */
  domain->n = (int)((domain->bmax - domain->bmin)/domain->bdel - 1e-5) + 1;

  //fprintf(stderr, "domain->n %d\n", domain->n);
  //getchar();

  /* barr: temperature array */
  if ((domain->barr = (double *) calloc((domain->n + 2), sizeof(double))) == NULL) {
    fprintf(stderr, "no memory! var: distr->domain->barr, type: double\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < domain->n+1; i++)
    domain->barr[i] = domain->bmin + i * domain->bdel;

  /* check beta array */
  if ( !(at_distr_domain__check_barr(domain) == 0) ) {
    fprintf(stderr, "check beta array\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  /* fix bmax to a bin boundary */
  domain->bmax = domain->bmin + domain->bdel * domain->n;

  return 0;

ERR:

  return -1;
}



void at_distr_domain__finish(at_distr_domain_t *domain)
{
  if (domain->barr != NULL) free(domain->barr);
}

void at_distr_domain__manifest(const at_distr_domain_t *domain, at_utils_manifest_t *manifest)
{
  FILE *fp = manifest->fp;

  //fprintf(stderr, "fp %p\n", manifest->fp); exit(1);

  /* bdel: bin size of beta */
  fprintf(fp, "distr->domain->bdel: double, %g\n", domain->bdel);

  /* n: number of temperature bins */
  fprintf(fp, "distr->domain->n: int, %4d\n", domain->n);

  /* barr: temperature array */
  at_utils_manifest__print_double_arr(manifest, domain->barr, domain->n+1, "distr->domain->barr");
}



int at_distr_domain__beta_to_index(const at_distr_domain_t *domain, double beta, int check)
{
  int j;

  if (beta > domain->bmin) {
    j = (int)((beta - domain->bmin)/domain->bdel);
  } else {
    j = -1;
  }

  if (check) {
    zcom_util__exit_if (j < 0 || j >= domain->n, "beta = %d, %g, range = (%g, %g, %g)\n",
        j, beta, domain->bmin, domain->bdel, domain->bmax);
  }

  return j;
}




#endif
