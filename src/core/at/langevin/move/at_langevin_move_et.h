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

#ifndef AT_LANGEVIN_MOVE_ET_H__
#define AT_LANGEVIN_MOVE_ET_H__




#include "../../mb/at_mb__def.h"
#include "../at_langevin__def.h"



double at_langevin_move__calc_et(
    at_langevin_t *langevin,
    at_mb_t *mb,
    int ib,
    double def_val,
    int check_av_energy);


/* deterministic part of the kT-based Langevin equation */
double at_langevin_move__calc_dkt_deterministic(
    at_langevin_t *langevin,
    at_mb_t *mb, 
    int ib,
    double time_step,
    double neg_dlnwf_dbeta,
    double current_energy,
    double *bin_av_energy,
    int cheap_av_energy);


#endif
