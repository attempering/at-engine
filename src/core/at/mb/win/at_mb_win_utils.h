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

#ifndef AT_MB_WIN_UTILS_H__
#define AT_MB_WIN_UTILS_H__



//#include "mb_win__def.h"
#include "../at_mb__def.h"

#include <stdio.h>


/* windows for unrestricted grid values
 * They can used for reconstructing energy histograms.
 *
 * Such windows are not designed to work with the window-accumulator module.
 */
void at_mb_win__make_unres_windows_for_grid_estimators(
    int n, const double *barr, double bdel,
    int bw_mod, double bw_del,
    int *js_grid_raw, int *jt_grid_raw);


/* windows for bin estimators
 * such as the bin-averaged internal energy
 *
 * Such windows are designed to work with window-accumulator module.
 * They are usually symmetric. */
void at_mb_win__make_windows_for_bin_estimators(
    int n,
    int symmetric,
    const int *js_grid_raw, const int *jt_grid_raw,
    int *js_bin, int *jt_bin);


/* windows for grid estimators
 * such as the internal energy and heat capacity at the grid points.
 *
 * Such windows are designed to work with window-accumulator module.
 * They are contained in the bin estimators */
void at_mb_win__make_res_windows_for_grid_estimators(int n,
    int *js_bin, int *jt_bin,
    int *js_grid_res, int *jt_grid_res);


int at_mb_win__init_bin2wins(at_mb_win_t *win);


#endif
