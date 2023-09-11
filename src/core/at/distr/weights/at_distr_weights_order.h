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

#ifndef AT_DISTR_WEIGHTS_ORDER_H__
#define AT_DISTR_WEIGHTS_ORDER_H__


#include "at_distr_weights__def.h"

/* return if the two weights contain difference
   in any of the parameters */
at_bool_t at_distr_weights__diff(
    at_distr_weights_t *w1,
    at_distr_weights_t *w2);

/* comparing the distributions of two replicas
   in terms of the order parameter
   
   This routine assumes that the order parameter, q_order,
   has been pre-computed for both replicas */
int at_distr_weights__compare(
    at_distr_weights_t *w1,
    at_distr_weights_t *w2);

double at_distr_weights__calc_q_order(
    at_distr_weights_t *w);


#endif
