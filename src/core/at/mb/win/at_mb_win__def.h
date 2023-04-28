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

#ifndef AT_MB_WIN__DEF_H__
#define AT_MB_WIN__DEF_H__



/* struct for mb->bin2wins */
typedef struct at_mb_win_ids_t_ {
  int count;
  int curr_id_;
  int *ids;
} at_mb_win_ids_t;


/* Three types of windows for different quantities 
 *
 * 1. unrestricted grid values
 *    e.g., reconstructed energy histograms
 *    Such quantities do not have to work with
 *    the window-accumulator module.
 *    The window boundary variables are js_grid_unres and jt_grid_unres.
 *    Such windows are not necessarily symmetric.
 *
 * 2. bin values
 *    e.g., bin-averaged internal energy
 *    Such quantities are supposed to work with
 *    the window-accumulator modules.
 *    The window boundary variables are js_bin and jt_bin.
 *    The windows are usually symmetric
 *    and are contained in the unrestricted grid windows
 *
 * 3. restricted grid values
 *    e.g., internal energy, heat capacity at the grid points
 *    Such quantities are supposed to work with
 *    the window-accumulator modules.
 *    The window boundary variables are js_grid_res and jt_grid_res.
 *    The windows are contained in the corresponding bin values
 *
 */

typedef struct at_mb_win_t_ {
  int n;
  int bwmod;
  double bwdel;

  int *js_grid_unres;
  int *jt_grid_unres;

  int *js_bin;
  int *jt_bin;

  int *js_grid_res;
  int *jt_grid_res;

  /* mapping from bin to affected windows */
  at_mb_win_ids_t *bin2wins;

} at_mb_win_t;


#endif
