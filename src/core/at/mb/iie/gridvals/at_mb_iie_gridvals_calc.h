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

#ifndef AT_MB_IIE_GRIDVALS_CALC_H__
#define AT_MB_IIE_GRIDVALS_CALC_H__

/* integral-identity-based estimators for thermodynamic quantities at the grid points */

#include "at_mb_iie_gridvals__def.h"

typedef struct at_mb_t_ at_mb_t;

/* calculate the estimated average energy Et, Cv and partition function
 * at all grid points */
void at_mb_iie_gridvals__calc(at_mb_t *mb);

void at_mb_iie_gridvals__calc_energy(at_mb_t *mb);

void at_mb_iie_gridvals__calc_cv(at_mb_t *mb);

void at_mb_iie_gridvals__calc_lnz(at_mb_t *mb);


#endif
