/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

#ifndef AT_SRC_H__
#define AT_SRC_H__



/* this header needs to be placed before zcom modules
 * so that we can determine whether `real` should be
 * defined or not depending on whether the context is
 * GROMACS */
#include "context/at_context.h"



/* source code of zcom modules */
#ifdef ADAPTIVE_TEMPERING_FOR_GROMACS
  #define ZCOM__HAVE_REAL 1
#endif

#include "../zcom/zcom__src.h"


#include "utils/at_utils__src.h"


/* include the source code of sub-modules */
#include "bias/at_bias__src.h"
#include "mb/at_mb__src.h"
#include "eh/at_eh__src.h"
#include "langevin/at_langevin__src.h"

#include "at_basic__src.h"
#include "at_main__src.h"


#endif
