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

#ifndef AT_MB_IIE_LR_CALC_H__
#define AT_MB_IIE_LR_CALC_H__



#include "at_mb_iie_lr__def.h"



/* initialization for a particular estimate
 * this includes setting up proper accumulators */
void at_mb_iie_lr__init_instance(at_mb_iie_lr_t *lr,
    int type, int win_div,
    int ib, int js, int jt);

/* collect moments */
void at_mb_iie_lr__collect_moments(at_mb_iie_lr_t *lr);

/* to compute the coefficients of linear combination
 * for the left-right IIE */
double at_mb_iie_lr__balance_moments(at_mb_iie_lr_t *lr);



#endif
