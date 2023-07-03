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

#ifndef AT_DRIVER_LANGEVIN__DEF_H__
#define AT_DRIVER_LANGEVIN__DEF_H__

#include "rng/at_driver_langevin_rng__def.h"
#include "zerofiller/at_driver_langevin_zerofiller__def.h"
#include "integrator/at_driver_langevin_integrator__def.h"

#include "../../distr/at_distr__def.h"
#include "../../mb/at_mb__def.h"


/* custom integration function for the following integral
 *
 *  Int_{beta_old to beta_new} E dbeta = ln Z(beta_old) - ln Z(beta_new)
 *
 * This function is mainly used to diagnose initial stability
 * problems related to Metropolisation correction with a large time step.
 *
 * The above integral over beta bins is used in the Metropolisation correction,
 * but with few data points typical in early stages of a simulation,
 * Applying such a correction can be counterproductive.
 * We can use a custom integration function that is known to be good
 * to see if the stability issue goes away. 
 *
 * For example, for solvable systems such as the Gaussian energy model
 * whose the partition function is known, we can use a function
 * that returns the exact difference of the partition function.
 *
 */
typedef double (*at_mb_integrate_func_t)(at_mb_t *mb, double beta_old, double beta_new);

/* parameters related to the Langevin equation */
typedef struct at_driver_langevin_t_
{
  double  dt;           /* time step for the temperature Langevin equation (without the Boltzmann factor) */
  double  dTmax;        /* maximum amount of temperature change in a step */
  int move_repeats;

  double  rejects;      /* total number of failed attempts */
  double  total;        /* total number of (corrected) attempts */
  double  n_exemption;  /* total number of exemptions */

  /* the Metropolisation correction makes sure the correctness
   * of the algorithm for a large time step */
  at_bool_t corrected;    /* whether to apply the Metropolisation correction */

  /* the following parameters help improve the initial stability
   * of the algorithm related to large time steps */
  at_bool_t no_skip;        /* whether to avoid crossing over unvisited bins */
  double  bin_min_visits; /* minimum number of visits before moving out of a bin */

#ifdef AT_DRIVER_LANGEVIN__CORR_BIN_MIN_VISITS
  double  corr_bin_min_visits; 
                          /* minimum number of visits (for every bin pass through) 
                             to activate the Metropolisation correction */
#endif

  int nst_suggest;

  at_driver_langevin_rng_t rng[1];

  at_driver_langevin_integrator_t integrator[1];

  at_mb_integrate_func_t integrate_func;

  at_distr_t *distr; /* reference handle */

  at_mb_t *mb; /* reference handle */

  char *file; /* IO file name */

  at_utils_log_t log[1]; /* module-specific logger */

} at_driver_langevin_t;


#endif
