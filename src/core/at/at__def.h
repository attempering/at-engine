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

#ifndef AT__DEF_H__
#define AT__DEF_H__



/* this header needs to be placed before zcom modules
 * so that we can determine whether `real` should be defined or not */
#include "utils/context/at_utils_context.h"


#include "utils/at_utils__def.h"
#include "distr/at_distr__def.h"
#include "mb/at_mb__def.h"
#include "eh/at_eh__def.h"
#include "driver/at_driver__def.h"


typedef struct at_t_ {

  double    beta;             // current beta

  double    energy;           // current coupling energy

  at_distr_t    distr[1];     // distribution

  at_mb_t       mb[1];        // multiple-bin estimator

  at_driver_t   driver[1];    // Langevin equation

  at_eh_t       eh[1];        // energy histogram

  at_utils_t    utils[1];     // utilities and everything else

} at_t;


#endif
