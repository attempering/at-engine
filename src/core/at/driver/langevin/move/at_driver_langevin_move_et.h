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

#ifndef AT_DRIVER_LANGEVIN_MOVE_ET_H__
#define AT_DRIVER_LANGEVIN_MOVE_ET_H__




#include "../../../mb/at_mb__def.h"
#include "../at_driver_langevin__def.h"
#include "../../../utils/at_utils.h"



double at_driver_langevin_move__calc_et(
    at_driver_langevin_t *langevin,
    int ib,
    double def_val,
    at_bool_t check_av_energy);


/* deterministic part of the kT-based Langevin equation */
double at_driver_langevin_move__calc_dkt_deterministic(
    at_driver_langevin_t *langevin,
    int ib,
    double time_step,
    double neg_dlnwf_dbeta,
    double current_energy,
    double *bin_av_energy,
    at_bool_t cheap_av_energy);


#endif
