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

#ifndef AT_MB_ACCUM__DEF_H__
#define AT_MB_ACCUM__DEF_H__


#include "../sm/at_mb_sm.h"
#include "../win/at_mb_win.h"


#include "winaccum/at_mb_accum_winaccum__def.h"


typedef struct at_mb_accum_t_ {
  int n; /* number of bins == distr->domain->n */

  /* regular accumulators */
  at_mb_sm_t *sums;

  double *win_total; /* total of sum.s over a multiple-bin temperature window */

  at_mb_accum_winaccum_t winaccum[1];

  at_mb_win_t *win; /* reference to mb->win */

} at_mb_accum_t;


#define AT_MB_ACCUM_MIN_SIZE 1e-6


#endif

