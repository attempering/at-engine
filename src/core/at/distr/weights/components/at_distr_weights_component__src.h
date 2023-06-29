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

#ifndef AT_DISTR_WEIGHTS_COMPONENT__SRC_H__
#define AT_DISTR_WEIGHTS_COMPONENT__SRC_H__


#include "at_distr_weights_components.h"
#include "../../domain/at_distr_domain.h"
#include "../../../../zcom/zcom.h"


static int at_distr_weights_component__parse_gaussian_params(
    at_distr_weights_component_t *c,
    char *params)
{
  if (params == NULL) {
    fprintf(stderr, "Error@at.distr.weights.components: component %d (gaussian) has no parameters\n",
        c->id);
    return -1;
  }

  //fprintf(stderr, "params: [%s], ptr %p, id %d, key %s %s:%d\n", params, c, c->id, c->key, __FILE__, __LINE__);
  //getchar();

  if (sscanf(params, "%lf,%lf", &c->beta0, &c->sigma) != 2) {
    fprintf(stderr, "Error@at.distr.weights.components: component %d (gaussian) does not have enough parameters\n",
        c->id);
    return -1;
  }

  c->inv_sigma_sqr = 1.0 / (c->sigma * c->sigma);

  return 0;
}



static int at_distr_weights_component__parse_str(
    at_distr_weights_component_t *c,
    char *comp_str)
{
  char *p;
  char *params = NULL;

  /* format 

    type,rel-weight[:parameters]

    Example 1:

      flat,1

    Example 2:

      gaussian,2.5:0.32,0.05
      
        (a Gaussian with relative weight being 2.5, centered at beta = 0.32, sigma = 0.05)

  */

  zcom_utils__str_to_lower(comp_str);

  p = strchr(comp_str, ':');

  if (p != NULL) {
    params = p + 1;
    *p = '\0';
  }

  /* get the relative weight */
  p = strchr(comp_str, ',');
  if (p != NULL) {
    const char *w_str = p + 1;
    *p = '\0';
    sscanf(w_str, "%lf", &c->w_rel);
  } else {
    c->w_rel = 1.0;
  }

  zcom_utils__strip(comp_str);

  if (strncmp(comp_str, "gauss", 5) == 0
   || strncmp(comp_str, "norm", 4) == 0) {

    c->type = AT_DISTR_WEIGHTS_COMPONENT_TYPE__GAUSSIAN;

    at_distr_weights_component__parse_gaussian_params(c, params);

  } else if (strncmp(comp_str, "flat", 4) == 0) {

    c->type = AT_DISTR_WEIGHTS_COMPONENT_TYPE__FLAT;

  } else {

    fprintf(stderr, "Error@at.distr.weights.components: invalid component type %s\n",
        comp_str);

    exit(1);

  }

  return 0;
}


static int at_distr_weights_component__cfg_init(
    at_distr_weights_component_t *c,
    int ic,
    zcom_cfg_t *cfg, at_bool_t verbose)
{
  char *key;
  zcom_ssm_t *ssm = cfg->ssm;
  char *comp_str = NULL;

  key = zcom_ssm__dup(ssm, "ensemble-component-000000000000000000000000");
  sprintf(key, "ensemble-component-%d", ic+1);

  if (0 != zcom_cfg__get(cfg, &comp_str, key, "%s")) {
    if (verbose) fprintf(stderr, "Info@at.distr.weights.components: assuming default component %d, key: %s\n",
        ic, key);

    comp_str = zcom_ssm__dup(ssm, "flat,1.0");
  }

  c->id = ic;
  c->key = key;

  at_distr_weights_component__parse_str(c, comp_str);

  return 0;
}


static int at_distr_weights_component__manifest(
    const at_distr_weights_component_t *c,
    at_utils_manifest_t *manifest)
{
  FILE *fp = manifest->fp;

  fprintf(fp, "%s: ", c->key);

  if (c->type == AT_DISTR_WEIGHTS_COMPONENT_TYPE__FLAT) {
    fprintf(fp, "flat, weight %lf", c->w_rel);
  } else if (c->type == AT_DISTR_WEIGHTS_COMPONENT_TYPE__GAUSSIAN) {
    fprintf(fp, "gaussian, weight %lf: %lf %lf",
        c->w_rel, c->beta0, c->sigma);
  }

  fprintf(fp, "\n");

  return 0;
}


static double at_distr_weights_component__calc_f_factor(
    const at_distr_weights_component_t *c, double beta, double *p_neg_df_dbeta)
{
  double f_comp = 0.0;

  if (c->type == AT_DISTR_WEIGHTS_COMPONENT_TYPE__FLAT) {

    f_comp = c->w_rel;
    *p_neg_df_dbeta = 0;

  } else if (c->type == AT_DISTR_WEIGHTS_COMPONENT_TYPE__GAUSSIAN) {

    double del_beta = beta - c->beta0;
    f_comp = c->w_rel * exp(-0.5 * del_beta * del_beta * c->inv_sigma_sqr);
    *p_neg_df_dbeta = f_comp * del_beta * c->inv_sigma_sqr;

  } else {

    fprintf(stderr, "Error@at.distr.weights.components: unknown component type %d for %s\n",
        c->id, c->key);
    exit(1);

  }

  return f_comp;
}

#endif
