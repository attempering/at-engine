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

#ifndef AT_MB_IIE_ET_CACHE_H__
#define AT_MB_IIE_ET_CACHE_H__

/* integral identity based estimator of the bin-averaged energy
 * along with cache
 */
#include "at_mb_iie_et__def.h"


typedef struct at_mb_t_ at_mb_t;
typedef struct at_mb_sm_t_ at_mb_sm_t;

/* calculate the estimated average energy Et of bin ib
 * using the integral identity estimator.
 *
 * Note that the calculated value is a bin-averaged one,
 * not one at either boundary of the bin.
 * */
double at_mb_iie_et__calc_et_cached(at_mb_iie_t *iie, int ib, int *quality);


void at_mb_iie_et_item_cache__clear(at_mb_iie_et_item_cache_t *cache);



#endif
