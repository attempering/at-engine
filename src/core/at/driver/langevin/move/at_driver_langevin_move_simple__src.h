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

#ifndef AT_DRIVER_LANGEVIN_MOVE_SIMPLE__SRC_H__
#define AT_DRIVER_LANGEVIN_MOVE_SIMPLE__SRC_H__



#include "at_driver_langevin_move_simple.h"



/* Langevin integration */
double at_driver_langevin__move_simple(
    at_driver_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta_old,
    int ib_old,
    double invwf,
    double neg_dlnwf_dbeta,
    double *bin_av_energy)
{
  const at_bool_t cheap_av_energy = AT__FALSE;
  const at_bool_t apply_dkt_max = AT__TRUE;

  at_driver_langevin_move_proposal_t proposal[1];
  double beta;
  at_distr_domain_t *domain = langevin->distr->domain;

  //fprintf(stderr, "1. beta %g, ib %d\n", beta_old, ib);

  if (!at_driver_langevin_move__check_min_visits(langevin, mb, beta_old)) {
    return beta_old;
  }

  //fprintf(stderr, "2. beta %g, ib %d\n", beta_old, ib);

  at_driver_langevin_move__propose(
      proposal,
      langevin,
      current_energy,
      beta_old, ib_old,
      invwf, neg_dlnwf_dbeta,
      cheap_av_energy,
      apply_dkt_max,
      bin_av_energy);

  //fprintf(stderr, "3. beta %g => %g, %g\n", beta_old, proposal->beta_new, *bin_av_energy);

  at_driver_langevin_move__moderate_stride(
      proposal, langevin);

  if (proposal->beta_new < domain->beta_max
   && proposal->beta_new > domain->beta_min) {
    beta = proposal->beta_new;
  } else {
    beta = proposal->beta_old;
    langevin->rejects += 1.0;
  }

  langevin->total += 1.0;

  return beta;
}



#endif
