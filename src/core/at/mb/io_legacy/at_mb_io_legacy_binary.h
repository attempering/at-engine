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

/* binary IO routines for at_mb_t */
#ifndef AT_MB_IO_LEGACY_BINARY_H__
#define AT_MB_IO_LEGACY_BINARY_H__

#include "../at_mb__def.h"
#include "../../driver/langevin/at_driver_langevin.h"


/* legacy flags */
#define   AT_MB__USE_WIN_ACCUM       0x00000001    /* use adaptive averaging */
#define   AT_MB__NEED_CV             0x00000002    /* compute heat capacity */
#define   AT_MB__USE_SYM_WINS        0x00000004    /* use symmetrical windows */
#define   AT_MB__USE_SINGLE_BIN      0x00000020    /* use the single-bin estimator */
#define   AT_MB__VERBOSE             0x00001000    /* be verbose */



/* read at_mb_t data as binary */
int at_mb__read_binary_legacy(
    at_mb_t *mb,
    at_driver_langevin_t *langevin,
    double *beta,
    const char *fname,
    int *pver);


/* write at_mb_t data as binary */
int at_mb__write_binary_legacy(
    at_mb_t *mb,
    at_driver_langevin_t *langevin,
    double beta,
    const char *fname,
    int ver);

#endif
