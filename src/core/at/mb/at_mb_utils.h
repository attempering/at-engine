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
#ifndef AT_MB_UTILS_H__
#define AT_MB_UTILS_H__

#include "at_mb__def.h"



/* write various averages to ze_file */
int at_mb__write_ze_file(at_mb_t *mb, const char *fname);

int at_mb__beta_to_index(at_mb_t *mb, double bet, int check);

/* add energy and bet */
void at_mb__add(at_mb_t *mb, double e, double bet,
      int *pib, double *pinvwf, double *ndlnwfdbeta);

/* recompute all average energy */
void at_mb__refresh_et(at_mb_t *mb, int reps);

#endif
