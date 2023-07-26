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


#include "at_distr_weights_component__src.h"
#include "at_distr_weights_components_order__src.h"



int at_distr_weights_components__conf_init(
    at_distr_weights_components_t *c,
    at_distr_domain_t *domain,
    at_utils_conf_t *conf)
{
  int ic;

  c->beta_min = domain->beta_min;
  c->beta_max = domain->beta_max;
  c->n = domain->n;

  if (conf->verbose) {
    at_utils_log__info(conf->log, "multiple-component distribution mode\n");
  }

  /* flat ensemble mode */
  at_utils_conf__get_int(conf,
      "ensemble-n-components",
      &c->n_components, 1,
      "n_components");

  if (c->n_components <= 0) {
    at_utils_log__error(conf->log, "invalid number of components %d\n",
        c->n_components);
    c->n_components = 0;
    c->components = NULL;
    return -1;
  }

  at_utils__new_arr(c->components, c->n_components, at_distr_weights_component_t);

  for (ic = 0; ic < c->n_components; ic++) {
    at_distr_weights_component__conf_init(c->components + ic, ic, conf);
  }

  return 0;
}


void at_distr_weights_components__manifest(
    const at_distr_weights_components_t *c,
    at_utils_manifest_t *manifest)
{
  int ic;

  at_utils_manifest__print_int(manifest, c->n_components,
      "distr->weights->components->n_components", "ensemble-n-components");

  for (ic = 0; ic < c->n_components; ic++) {
    at_distr_weights_component__manifest(c->components + ic, manifest);
  }

}



void at_distr_weights_components__finish(at_distr_weights_components_t *c)
{
  int ic;

  for (ic = 0; ic < c->n_components; ic++) {
    at_distr_weights_component__finish(&c->components[ic]);
  }

  if (c->components != NULL) {
    free(c->components);
  }

  memset(c, '\0', sizeof(*c));
}



/* compute the ensemble f factor
 * f: f(beta);
 * *p_neg_df_dbeta: -df(beta)/dbeta;
 */
double at_distr_weights_components__calc_f_factor(
    const at_distr_weights_components_t *c,
    double beta, double *p_neg_df_dbeta,
    at_utils_log_t *log)
{
  double f, neg_df_dbeta;
  int ic;

  if (c->n_components > 0) {

    f = 0.0;
    neg_df_dbeta = 0.0;

    for (ic = 0; ic < c->n_components; ic++) {
      double f_comp, neg_df_dbeta_comp;

      f_comp = at_distr_weights_component__calc_f_factor(
          c->components + ic, beta, &neg_df_dbeta_comp, log);

      f += f_comp;
      neg_df_dbeta += neg_df_dbeta_comp;

    }

  } else {

    f = 1.0;
    neg_df_dbeta = 0.0;

  }

  if (p_neg_df_dbeta != NULL) {
    *p_neg_df_dbeta = neg_df_dbeta;
  }

  return f;
}



void at_distr_weights_components__basic_init(
    at_distr_weights_components_t *c,
    at_distr_domain_t *domain,
    int n_components)
{
  c->n_components = n_components;

  if (n_components > 0) {
    zcom_utils__new(c->components, n_components, at_distr_weights_component_t);
  } else {
    c->components = NULL;
  }

  c->beta_min = domain->beta_min;
  c->beta_max = domain->beta_max;
  c->n = domain->n;

}



#endif
