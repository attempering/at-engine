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
#ifndef AT_MB_IO_H__
#define AT_MB_IO_H__


#include "at_mb_io_binary.h"

#include "../../utils/at_utils.h"
#include "../../driver/at_driver.h"


/* load previous mb data */
int at_mb__read(at_mb_t *mb);

/* prepare and write mb data  */
int at_mb__write(at_mb_t *mb);

#endif
