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

#ifndef AT_DRIVER_LANGEVIN_MOVE_BASIC__SRC_H__
#define AT_DRIVER_LANGEVIN_MOVE_BASIC__SRC_H__



#include "at_driver_langevin_move_basic.h"

/* implementation headers */
#include "../../../distr/at_distr.h"
#include "../../../mb/at_mb.h"
#include "../../../../zcom/zcom.h"
#include "at_driver_langevin_move_et.h"


/* to dump debugging details, set it to 1 */
int at_driver_langevin_move__debug__ = 0;

const double at_driver_langevin_move__stride_moderation_frac = 0.5;



/* check if the minimum stay requirement has been met */
int at_driver_langevin_move__check_min_visits(
    at_driver_langevin_t *langevin,
    at_mb_t *mb,
    double beta_old)
{
  int ib = at_mb__beta_to_index(mb, beta_old, 0);

  if (ib >= 0 && ib < langevin->distr->domain->n) {
    if (mb->visits[ib] < langevin->bin_min_visits) {
      //fprintf(stderr, "%d %g %g\n", ib, mb->visits[ib], langevin->bin_min_visits);
      return 0;
    }
  }

  return 1; // default is pass
}



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
    double *bin_av_energy)
{
  double rand_mag, noise;

  proposal->current_energy = current_energy;

  proposal->invwf = invwf;

  proposal->beta_old = beta_old;

  proposal->kt_old = 1.0 / beta_old;

  proposal->time_step = langevin->dt * langevin->distr->boltz;

  rand_mag = proposal->kt_old * sqrt(2.0 * proposal->time_step);

  noise = zcom_mtrng__randgaus(langevin->rng->mtrng);

  proposal->dkt_deterministic = at_driver_langevin_move__calc_dkt_deterministic(
      langevin,
      ib,
      proposal->time_step,
      neg_dlnwf_dbeta,
      current_energy,
      bin_av_energy,
      cheap_av_energy);

  proposal->dkt = proposal->dkt_deterministic + rand_mag * noise;

  if (apply_dkt_max) {
    double dkt_max = langevin->dTmax * langevin->distr->boltz;

    if (proposal->dkt > dkt_max) {
      proposal->dkt = dkt_max;
    } else if (proposal->dkt < -dkt_max) {
      proposal->dkt = -dkt_max;
    }

  }

  proposal->kt_new = proposal->kt_old + proposal->dkt;

  proposal->beta_new = 1.0 / proposal->kt_new;

  return 0;
}



at_bool_t at_driver_langevin_move__moderate_stride(
    langevin_move_proposal_t *proposal,
    at_driver_langevin_t *langevin)
{
  int ib_old, ib_new, ib;
  at_bool_t moderated = 0;
  at_distr_domain_t *domain = langevin->distr->domain;
  const double frac = at_driver_langevin_move__stride_moderation_frac;

  if (!langevin->no_skip) {
    return moderated;
  }

  ib_old = at_distr_domain__beta_to_index(domain, proposal->beta_old, 0);

  ib_new = at_distr_domain__beta_to_index(domain, proposal->beta_new, 0);

  proposal->ib_old = ib_old;
  proposal->beta_new_prop = proposal->beta_new;
  proposal->ib_new_prop = ib_new;
  proposal->ib_new = ib_new;

  if (ib_new > ib_old) {

    for (ib = ib_old + 1; ib < ib_new && ib < domain->n; ib++) {
      // check if ib is an unvisited bin
      if (langevin->mb->visits[ib] <= 0.0) {
        // stop at the center of the unvisited bin
        proposal->beta_new = domain->beta_min + (ib + frac) * domain->beta_del;
        proposal->ib_new = ib;
        moderated = AT__TRUE;
        break;
      }
    }

  } else if (ib_new < ib_old) {

    for (ib = ib_old - 1; ib > ib_new && ib >= 0; ib--) {
      // check if ib is an unvisited bin
      if (langevin->mb->visits[ib] <= 0.0) {
        // stop at the center of the unvisited bin
        proposal->beta_new = domain->beta_min + (ib + 1 - frac) * domain->beta_del;
        proposal->ib_new = ib;
        moderated = AT__TRUE;
        break;
      }
    }

  }

  // if beta_new has been changed,
  // change the corresponding kt_new respectively
  //
  // we do not have to change dkt, however,
  // as it is no longer needed in subsequent steps
  //
  if (moderated) {
    proposal->kt_new = 1.0 / proposal->beta_new;
  }

  if (moderated && proposal->ib_new_prop == 0) {
    fprintf(stderr, "Info@at.driver.langevin.move: a trip from %g (%d) to %g (%d) moderated to %g (%d)\n",
        proposal->beta_old, ib_old,
        proposal->beta_new_prop, proposal->ib_new_prop,
        proposal->beta_new, proposal->ib_new);
  }

  return moderated;
}



#endif
