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





int at_distr_domain__cfg_init(at_distr_domain_t *domain, zcom_cfg_t *cfg, int silent)
{
  int i;

  /* read bmin and bmax from the configuration file */

  if (cfg != NULL && 0 != zcom_cfg__get(cfg, &domain->bmin, "beta_min", "%lf")) {
    fprintf(stderr, "missing var: domain->bmin, key: beta_min, fmt: %%lf\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  if ( !(domain->bmin >= 0.0) ) {
    fprintf(stderr, "domain->bmin: failed validation: domain->bmin %g > 1e-6\n",
        domain->bmin);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }


  if (cfg != NULL && 0 != zcom_cfg__get(cfg, &domain->bmax, "beta_max", "%lf")) {
    fprintf(stderr, "missing var: domain->bmax, key: beta_max, fmt: %%lf\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  //printf("domain->bmin %g, domain->bmax %g\n", domain->bmin, domain->bmax); getchar();

  if ( !(domain->bmax >= domain->bmin) ) {
    fprintf(stderr, "domain->bmax: failed validation: domain->bmax %g > domain->bmin %g\n",
        domain->bmax, domain->bmin);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* bdel: bin size of beta */
  domain->bdel = 0.0001;

  if (cfg != NULL && 0 != zcom_cfg__get(cfg, &domain->bdel, "beta_del", "%lf")) {
    fprintf(stderr, "missing var: domain->bdel, key: beta_del, fmt: %%lf\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  if ( !(domain->bdel > 1e-6) ) {
    fprintf(stderr, "domain->bdel: failed validation: domain->bdel > 1e-6\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* n: number of temperature bins */
  domain->n = (int)((domain->bmax - domain->bmin)/domain->bdel - 1e-5) + 1;
  /* barr: temperature array */
  if ((domain->barr = (double *) calloc((domain->n + 2), sizeof(double))) == NULL) {
    fprintf(stderr, "no memory! var: domain->barr, type: double\n");
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
  if (domain->barr      != NULL) free(domain->barr);
}

void at_distr_domain__manifest(at_distr_domain_t *domain, at_utils_manifest_t *manifest)
{

}

#endif
