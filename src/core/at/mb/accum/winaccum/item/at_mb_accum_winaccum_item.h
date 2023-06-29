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

#ifndef AT_MB_ACCUM_WINACCUM_ITEM_H__
#define AT_MB_ACCUM_WINACCUM_ITEM_H__


#include "../at_mb_accum_winaccum__def.h"
#include "at_mb_accum_winaccum_item_utils.h"
#include "io/at_mb_accum_winaccum_item_io.h"

#include "../../../at_mb__def.h"
#include "../../../win/at_mb_win.h"
#include "../../../shk/at_mb_shk.h"
#include <stdio.h>



void at_mb_accum_winaccum_item__add(
    at_mb_accum_winaccum_item_t *item,
    int ib,
    double invwf, double e, int do_2nd,
    at_mb_shk_t *shk,
    double total_visits);



void at_mb_accum_winaccum_item__init(at_mb_accum_winaccum_item_t *item,
    int i, at_mb_win_t *win);

void at_mb_accum_winaccum_item__clear(at_mb_accum_winaccum_item_t *item);

void at_mb_accum_winaccum_item__finish(at_mb_accum_winaccum_item_t *item);


#endif
