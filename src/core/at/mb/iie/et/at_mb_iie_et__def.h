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

#ifndef AT_MB_IIE_ET__DEF_H__
#define AT_MB_IIE_ET__DEF_H__

#include "cache/at_mb_iie_et_cache__def.h"

typedef struct at_mb_t_ at_mb_t;



typedef struct at_mb_iie_et_item_t_ {
  int ib;

  double value; /* the value of et */

  double imbalance;

  int quality; /* the current value is of good quality */

  at_mb_iie_et_item_cache_t cache[1];

} at_mb_iie_et_item_t;


typedef struct at_mb_iie_et_t_ {
  int n; /* same as distr->domain->n */
  at_mb_iie_et_item_t *items;
  at_mb_iie_et_cache_params_t cache_params[1];
  int cache_hit; /* whether the latest call to at_mb_iie_et__calc_et_cached() is a cache hit */
} at_mb_iie_et_t;



#endif
