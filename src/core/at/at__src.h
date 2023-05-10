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

#ifndef AT__SRC_H__
#define AT__SRC_H__



/* this header needs to be placed before zcom modules
 * so that we can determine whether `real` should be
 * defined or not depending on whether the context is
 * GROMACS */

#include "at.h"

#include "params/at_params__src.h"
#include "utils/at_utils__src.h"
#include "distr/at_distr__src.h"
#include "mb/at_mb__src.h"
#include "eh/at_eh__src.h"
#include "driver/at_driver__src.h"

#include "at_basic__src.h"
#include "at_misc__src.h"
#include "at_io__src.h"
#include "at_move__src.h"


#endif
