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

#ifndef AT_DISTR_WEIGHTS_COMPONENT__DEF_H__
#define AT_DISTR_WEIGHTS_COMPONENT__DEF_H__

#include "../../../context/at_context__def.h"

enum {
  AT_DISTR_WEIGHTS_COMPONENT_TYPE__FLAT = 0,
  AT_DISTR_WEIGHTS_COMPONENT_TYPE__GAUSSIAN = 1,
  AT_DISTR_WEIGHTS_COMPONENT_TYPE__COUNT
};


typedef struct at_distr_weights_component_t_ {

  int id;

  char *key; /* key from configuration file */

  int type; /* one of AT_DISTR_WEIGHTS_COMPONENT_TYPE__XXX, %packed */

  double w_rel; /* relative weight, %packed */

  double beta0; /* %packed */

  double sigma; /* %packed */

  double inv_sigma_sqr;

} at_distr_weights_component_t;




#endif
