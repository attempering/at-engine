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

/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT_MB_H__
#define AT_MB_H__

/* definition of at_mb_t */
#include "at_mb__def.h"

/* simple accumulator */
#include "sm/at_mb_sm.h"

/* shrinking rate of the adaptive averaging scheme */
#include "shk/at_mb_shk.h"

/* window set-up functions */
#include "win/at_mb_win.h"

/* accumulators */
#include "accum/at_mb_accum.h"

/* basic routines, such as zcom_cfg__open()/close() */
#include "at_mb_basic.h"

/* miscellaneous routines */
#include "at_mb_utils.h"

/* IO routines */
#include "io/at_mb_io.h"

/* Legacy IO routines */
#include "io_legacy/at_mb_io_legacy.h"

/* integral-identity-based estimators */
#include "iie/at_mb_iie.h"

#endif
