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

#ifndef AT_LANGEVIN_MOVE_H__
#define AT_LANGEVIN_MOVE_H__



#include "../../../mb/at_mb__def.h"
#include "../at_driver_langevin__def.h"
#include "../../../../zcom/zcom.h"



/* estimate `bin_av_energy` at the current temperature `bet`
 * calculate the new beta after integrating
 * the Langevin equation based on kT = 1/beta;
 * neg_dlnwf_dbeta = -d[ln(w*f)]/d(beta) */
double at_driver_langevin__move(
    at_driver_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    double *bin_av_energy);

/* Langevin equation without Metropolis correction */
double at_driver_langevin__move_simple(
    at_driver_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    double *bin_av_energy);

/* Langevin equation with Metropolisation correction */
double at_driver_langevin__move_corrected(
    at_driver_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    double *bin_av_energy);

/* return the acceptance ratio so far (for the corrected move) */
double at_driver_langevin_move__get_acceptance_ratio(at_driver_langevin_t *langevin);


int at_driver_langevin_move__debug__;

#endif
