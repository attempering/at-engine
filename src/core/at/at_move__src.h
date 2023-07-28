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

#ifndef AT_MOVE__SRC_H__
#define AT_MOVE__SRC_H__


#include "at_misc.h"
#include "at_move.h"

#include "utils/at_utils.h"



void at__update_force_scale(at_t *at)
{
  at->force_scale = at->beta / at__temp_to_beta(at, at->utils->thermostat_temp);
}



at_bool_t at__do_tempering_on_step(at_t *at, at_llong_t step, at_bool_t value_on_neg_nst_temp)
{
  int nst_tempering = at->driver->nst_tempering;

  if (nst_tempering > 0 && (step % nst_tempering == 0)) {
    return AT__TRUE;
  } else if (nst_tempering <= 0) {
    return value_on_neg_nst_temp;
  }

  return AT__FALSE;
}



int at__move(at_t *at, const at_params_step_t *step_params)
{
  double invwf = 1.0;
  double temp_before, temp_after;
  double av_energy = 0.0, neg_dlnwf_dbeta;
  int ib, repeat;

  temp_before = at__beta_to_temp(at, at->beta);

  ib = at_distr__beta_to_index(at->distr, at->beta, AT__TRUE);

  /* update energy data, change at->beta */
  /* repeat several times to change the temperature */
  for (repeat = 0; repeat < at->driver->move_repeats; repeat++) {
    /* 1. deposit the current energy and temperature */
    at_mb__add(at->mb, at->energy, at->beta, &ib, &invwf, &neg_dlnwf_dbeta);

    /* 2. add the current data point to the energy histogram */
    at_eh__add(at->eh, ib, at->energy);

    /* 3. use the Langevin equation to update the temperature */
    at->beta = at_driver_langevin__move(at->driver->langevin, at->mb, at->energy, at->beta,
        ib, invwf, neg_dlnwf_dbeta, &av_energy);
  }

  temp_after = at__beta_to_temp(at, at->beta);

  // update at->force_scale according to the new at->beta
  at__update_force_scale(at);

  // refresh Et
  if (at__do_output_on_step(step_params, at->mb->nst_refresh, AT__TRUE)) {
    at_mb__refresh_et(at->mb);
  }

  at__write_trace(at, step_params, ib, invwf, temp_before, temp_after, av_energy);

  at__output(at, step_params);

  return 0;
}



int at__step(at_t *at, double energy, at_llong_t step, at_params_step_t *step_params)
{
  // default parameters
  at_params_step_t stock_step_params[1] = {
    {
      0,
      AT__FALSE, // is_first_step
      AT__FALSE, // is_last_step
      AT__FALSE, // do_trace
      AT__FALSE  // flush_output
    }
  };

  if (step_params == NULL) {
    step_params = stock_step_params;
  }


  step_params->step = step;

  at->energy = energy;

  return at__move(at, step_params);
}



double at__get_move_acceptance_ratio(
    const at_t *at)
{
  return at_driver_langevin_move__get_acceptance_ratio(at->driver->langevin);
}


#endif
