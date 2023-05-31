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

#ifndef AT_DISTR_WEIGHTS_COMPONENTS__DEF_H__
#define AT_DISTR_WEIGHTS_COMPONENTS__DEF_H__

#include "../../../context/at_context__def.h"

enum {
  AT_DISTR_WEIGHTS_COMPONENT_TYPE__FLAT = 0,
  AT_DISTR_WEIGHTS_COMPONENT_TYPE__GAUSSIAN = 1,
  AT_DISTR_WEIGHTS_COMPONENT_TYPE__COUNT
};


typedef struct at_distr_weights_component_t_ {

  int id;

  const char *key;

  int type;

  double w_rel; /* relative weight */

  double beta0;

  double sigma;

  double inv_sigma_sqr;

} at_distr_weights_component_t;



typedef struct at_distr_weights_components_t_ {

  double    beta_min;
  double    beta_max;
  int       n;

  int n_components;
  at_distr_weights_component_t *components;

} at_distr_weights_components_t;



#endif
