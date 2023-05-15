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

#ifndef AT_DRIVER_LANGEVIN_MOVE_BASIC_H__
#define AT_DRIVER_LANGEVIN_MOVE_BASIC_H__




#include "../../../mb/at_mb__def.h"
#include "../at_driver_langevin__def.h"
#include "../../../../zcom/zcom.h"
#include "at_driver_langevin_move_et.h"



typedef struct langevin_move_proposal_t_ {
  double current_energy;
  double invwf;
  double time_step;
  double kt_old;
  double dkt_deterministic;
  double dkt;
  double kt_new;
  double beta_old;
  double beta_new;
  double beta_new_prop;
  int ib_old;
  int ib_new;
  int ib_new_prop;
} langevin_move_proposal_t;


/* check if the minimum stay requirement has been met */
int at_driver_langevin_move__check_min_visits(
    at_driver_langevin_t *langevin,
    at_mb_t *mb,
    double beta_old);

int at_driver_langevin_move__propose(
    langevin_move_proposal_t *proposal,
    at_driver_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta_old,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    int cheap_av_energy,
    int apply_dkt_max,
    double *bin_av_energy);

/* moderate the stride in the beta space */
at_bool_t at_driver_langevin_move__moderate_stride(
    langevin_move_proposal_t *proposal,
    at_driver_langevin_t *langevin);

#endif
