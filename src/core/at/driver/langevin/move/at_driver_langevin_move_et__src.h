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

#ifndef AT_DRIVER_LANGEVIN_MOVE_ET__SRC_H__
#define AT_DRIVER_LANGEVIN_MOVE_ET__SRC_H__



#include "at_driver_langevin_move_et.h"

/* implementation headers */
#include "../../../mb/at_mb.h"



static double at_driver_langevin_move__calc_et_single_bin(
    at_driver_langevin_t *langevin,
    int ib,
    double def_val)
{
  double et;
  at_mb_sm_t *sm = at_mb_accum__get_proper_sums(langevin->mb->accum, ib, ib);

  if (sm->s > AT_MB_ACCUM_MIN_SIZE) {
    et = at_mb_sm__get_mean(sm, AT_MB_ACCUM_MIN_SIZE);
  } else {
    et = def_val;
  }

  return et;
}



static double at_driver_langevin_move__calc_et_iie(
    at_driver_langevin_t *langevin,
    int ib,
    double def_val)
{
  at_mb_iie_t *iie = langevin->mb->iie;
  int quality = 0;
  double et = at_mb_iie_et__calc_et_cached(iie, ib, &quality);

  if (at_driver_langevin_move__debug__ >= 2) {
    fprintf(stderr, "at_driver_langevin_move__calc_et_iie() %s:%d\n", __FILE__, __LINE__);
    fprintf(stderr, "  et %g (default %g)\n", et, def_val);
    fprintf(stderr, "  ib %d\n", (int) ib);
    fprintf(stderr, "  quality %d\n", quality);
    fprintf(stderr, "  cache enabled %d, hit %d\n",
        iie->et->cache_params->enabled, iie->et->cache_hit);
    fprintf(stderr, "\n");
  }

  //if ((ib == 0 || ib == iie->et->n - 1) && et == 0.0) {
  //  fprintf(stderr, "Info@at: boundary ib %d value is zero, quality %d\n", ib, lr->quality);
  //}

  //fprintf(stderr, "iie %p, %s:%d\n", iie, __FILE__, __LINE__);
  //fprintf(stderr, "lr %p, %s:%d\n", lr, __FILE__, __LINE__);
  //getchar();

  if (quality) {
    return et;
  } else {
    return def_val;
  }

}



double at_driver_langevin_move__calc_et(
    at_driver_langevin_t *langevin,
    int ib,
    double def_val,
    int cheap_av_energy)
{
  if (cheap_av_energy) {

    return at_driver_langevin_move__calc_et_single_bin(
        langevin,
        ib,
        def_val);

  } else {

    return at_driver_langevin_move__calc_et_iie(
        langevin,
        ib,
        def_val);

  }

}



/* deterministic part of the kT-based Langevin equation */
double at_driver_langevin_move__calc_dkt_deterministic(
    at_driver_langevin_t *langevin,
    int ib,
    double time_step,
    double neg_dlnwf_dbeta,
    double current_energy,
    double *bin_av_energy,
    int cheap_av_energy)
{
  double delta;

  *bin_av_energy = at_driver_langevin_move__calc_et(
      langevin,
      ib,
      current_energy,
      cheap_av_energy);

  delta = (current_energy - *bin_av_energy + neg_dlnwf_dbeta);

  if (at_driver_langevin_move__debug__ >= 2) {
    fprintf(stderr, "at_driver_langevin_move__calc_dkt_deterministic() %s:%d\n", __FILE__, __LINE__);
    fprintf(stderr, "  cheap_av_energy %d\n", cheap_av_energy);
    fprintf(stderr, "  ib %d\n", (int) ib);
    fprintf(stderr, "  current_energy %g\n", current_energy);
    fprintf(stderr, "  bin_av_energy %g\n", *bin_av_energy);
    fprintf(stderr, "  neg_dlnwf_dbeta %g\n", neg_dlnwf_dbeta);
    fprintf(stderr, "  delta %g\n", delta);
    fprintf(stderr, "  dkt %g\n", delta * time_step);
    fprintf(stderr, "\n");
  }

  return delta * time_step;
}



#endif
