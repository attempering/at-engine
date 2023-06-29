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

#ifndef AT_MB_ACCUM_BASIC_H__
#define AT_MB_ACCUM_BASIC_H__


#include "at_mb_accum__def.h"
#include "../at_mb__def.h"
#include "../win/at_mb_win.h"
#include "../shk/at_mb_shk.h"
#include "../../../zcom/zcom.h"
#include <stdio.h>


at_mb_sm_t *at_mb_accum__get_sums(at_mb_accum_t *accum, int j);

at_mb_sm_t *at_mb_accum__get_proper_sums(at_mb_accum_t *accum, int win_id, int ib);

double at_mb_accum__get_window_visits(at_mb_accum_t *accum, int win_id);

void at_mb_accum__add(at_mb_accum_t *accum, int j, double invwf, double e, int do_2nd,
    at_mb_shk_t *shk, double total_visits);

void at_mb_accum__calc_win_total(at_mb_accum_t *accum);

at_mb_sm_t *at_mb_accum__get_proper_sums0_and_winaccum_item(
    at_mb_accum_t *accum, int win_id,
    at_mb_accum_winaccum_item_t **winaccum_item);

void at_mb_accum__normalize(at_mb_accum_t *accum);

void at_mb_accum__cfg_init(at_mb_accum_t *accum, int n, at_mb_win_t *win,
    zcom_cfg_t *cfg, at_bool_t verbose);

void at_mb_accum__clear(at_mb_accum_t *accum);

void at_mb_accum__finish(at_mb_accum_t *accum);

void at_mb_accum__manifest(const at_mb_accum_t *accum,
    at_utils_manifest_t *manifest);

#endif
