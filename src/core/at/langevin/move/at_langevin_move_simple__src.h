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

#ifndef AT_LANGEVIN_MOVE_SIMPLE__SRC_H__
#define AT_LANGEVIN_MOVE_SIMPLE__SRC_H__



#include "at_langevin_move_basic.h"



/* Langevin integration */
double at_langevin__move_simple(
    at_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta_old,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    double *bin_av_energy)
{
  const int cheap_av_energy = 0;
  const int apply_dkt_max = 1;

  langevin_move_proposal_t proposal[1];
  double beta;

  //fprintf(stderr, "1. beta %g, ib %d\n", beta_old, ib);

  if (!at_langevin_move__check_min_visits(langevin, mb, beta_old)) {
    return beta_old;
  }

  //fprintf(stderr, "2. beta %g, ib %d\n", beta_old, ib);

  at_langevin_move__propose(
      proposal,
      langevin, mb,
      current_energy,
      beta_old, ib,
      invwf, neg_dlnwf_dbeta,
      cheap_av_energy,
      apply_dkt_max,
      bin_av_energy);

  //fprintf(stderr, "3. beta %g => %g, %g\n", beta_old, proposal->beta_new, *bin_av_energy);

  at_langevin_move__moderate_stride(
      proposal,
      langevin,
      mb);

  if (proposal->beta_new < mb->bmax
   && proposal->beta_new > mb->bmin) {
    beta = proposal->beta_new;
  } else {
    beta = proposal->beta_old;
    langevin->rejects += 1.0;
  }

  langevin->total += 1.0;

  return beta;
}



#endif
