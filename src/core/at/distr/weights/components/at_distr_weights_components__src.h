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

#ifndef AT_DISTR_WEIGHTS_COMPONENTS__SRC_H__
#define AT_DISTR_WEIGHTS_COMPONENTS__SRC_H__


#include "at_distr_weights_components.h"
#include "../../domain/at_distr_domain.h"
#include "../../../../zcom/zcom.h"


#include "at_distr_weights_component__src.h"



int at_distr_weights_components__cfg_init(
    at_distr_weights_components_t *c,
    at_distr_domain_t *domain, zcom_cfg_t *cfg, at_bool_t verbose)
{
  int ic;

  c->beta_min = domain->beta_min;
  c->beta_max = domain->beta_max;
  c->n = domain->n;

  if (verbose) fprintf(stderr, "Info@at.distr.weights: multiple-component distribution mode\n");

  /* flat ensemble mode */
  c->n_components = 1;
  if (0 != zcom_cfg__get(cfg, &c->n_components, "ensemble-n-components", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.distr.weights.components: assuming a single component, key: ensemble-n-components\n");
  }

  if (c->n_components <= 0) {
    if (verbose) fprintf(stderr, "Error@at.distr.weights.components: invalid number of components %d\n",
        c->n_components);
    c->n_components = 0;
    c->components = NULL;
    return -1;
  }

  c->components = (at_distr_weights_component_t *) calloc(c->n_components, sizeof(at_distr_weights_component_t));
  if (c->components == NULL) {
    fprintf(stderr, "Fatal@at.distr.weights.components: no memory for components %d\n", c->n_components);
    exit(-1);
  }

  for (ic = 0; ic < c->n_components; ic++) {
    at_distr_weights_component__cfg_init(c->components + ic, ic, cfg, verbose);
  }

  return 0;
}


void at_distr_weights_components__manifest(
    const at_distr_weights_components_t *c,
    at_utils_manifest_t *manifest)
{
  int ic;

  at_utils_manifest__print_int(manifest, c->n_components, "distr->weights->components->n_components", "ensemble-n-components");

  for (ic = 0; ic < c->n_components; ic++) {
    at_distr_weights_component__manifest(c->components + ic, manifest);
  }

}



void at_distr_weights_components__finish(at_distr_weights_components_t *c)
{
  if (c->components != NULL) {
    free(c->components);
  }
}



/* compute the ensemble f factor
 * f: f(beta);
 * *p_neg_df_dbeta: -df(beta)/dbeta;
 */
double at_distr_weights_components__calc_f_factor(
    const at_distr_weights_components_t *c,
    double beta, double *p_neg_df_dbeta)
{
  double f = 0, neg_df_dbeta = 0;
  int ic;

  for (ic = 0; ic < c->n_components; ic++) {
    double f_comp, neg_df_dbeta_comp;

    f_comp = at_distr_weights_component__calc_f_factor(
        c->components + ic, beta, &neg_df_dbeta_comp);

    f += f_comp;
    neg_df_dbeta += neg_df_dbeta_comp;

  }

  if (p_neg_df_dbeta != NULL) {
    *p_neg_df_dbeta = neg_df_dbeta;
  }

  return f;
}



#endif