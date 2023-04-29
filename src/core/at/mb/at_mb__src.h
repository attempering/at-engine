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

#ifndef AT_MB__SRC_H__
#define AT_MB__SRC_H__



/* external dependencies */
#include "../../zcom/zcom__src.h"


/* simple accumulator */
#include "sm/at_mb_sm__src.h"

/* shrinking rate of the adaptive averaging scheme */
#include "shk/at_mb_shk__src.h"

/* window set-up functions */
#include "win/at_mb_win__src.h"

/* accumulators */
#include "accum/at_mb_accum__src.h"

#include "at_mb_basic__src.h"

#include "at_mb_utils__src.h"

/* mb_io_binary__src.h is included within */
#include "io/at_mb_io__src.h"

/* integral-identity estimators */
#include "iie/at_mb_iie__src.h"

#endif
