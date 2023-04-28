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

/* binary IO routines for at_mb_t */
#ifndef AT_MB_IO_BINARY_H__
#define AT_MB_IO_BINARY_H__

#include "../at_mb__def.h"
#include "../../langevin/at_langevin.h"


/* at_mb__read_binary: read at_mb_t data as binary */
int at_mb__read_binary(
    at_mb_t *mb,
    at_langevin_t *langevin,
    double *beta,
    const char *fname,
    int *pver);


/* at_mb__write_binary: write at_mb_t data as binary */
int at_mb__write_binary(
    at_mb_t *mb,
    at_langevin_t *langevin,
    double beta,
    const char *fname,
    int ver);

#endif
