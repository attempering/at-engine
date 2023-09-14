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

#ifndef AT_DISTR_WEIGHTS_COMPOSITE_ORDER__SRC_H__
#define AT_DISTR_WEIGHTS_COMPOSITE_ORDER__SRC_H__


#include "at_distr_weights_composite_order.h"


static at_bool_t at_distr_weights_component__diff(
    at_distr_weights_component_t *c1,
    at_distr_weights_component_t *c2)
{
  if (c1->w_rel != c2->w_rel) {
    return AT__TRUE;
  }

  if (c1->type != c2->type) {
    return AT__TRUE;
  }

  if (c1->beta0 != c2->beta0) {
    return AT__TRUE;
  }

  if (c1->sigma != c2->sigma) {
    return AT__TRUE;
  }

  return AT__FALSE;
}


at_bool_t at_distr_weights_composite__diff(
    at_distr_weights_composite_t *c1,
    at_distr_weights_composite_t *c2)
{
  int ic;

  if (c1->n_components != c2->n_components) {
    return AT__TRUE;
  }

  for (ic = 0; ic < c1->n_components; ic++) {
    if (at_distr_weights_component__diff(
            &c1->components[ic],
            &c2->components[ic])) {
      return AT__TRUE;
    }
  }

  return AT__FALSE;
}



#endif
