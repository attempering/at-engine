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

#ifndef AT_DISTR__DEF_H__
#define AT_DISTR__DEF_H__

#include "domain/at_distr_domain__def.h"
#include "weights/at_distr_weights__def.h"
#include "bias/at_distr_bias__def.h"

/* parameters for the overall beta disribution */
typedef struct at_distr_t_ {

  at_distr_domain_t domain[1];

  at_distr_weights_t weights[1];

  at_distr_bias_t   bias[1];

} at_distr_t;

#endif