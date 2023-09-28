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

#ifndef AT_DRIVER_LANGEVIN_INTEGRATOR_H__
#define AT_DRIVER_LANGEVIN_INTEGRATOR_H__



/* integrator of the estimated averager over a temperature range */



#include "../../../distr/at_distr__def.h"
#include "../../../mb/at_mb__def.h"



int at_driver_langevin_integrator__init(
    at_driver_langevin_integrator_t *intgr,
    at_distr_t *distr,
    at_mb_t *mb,
    at_bool_t use_zerofiller,
    at_bool_t use_visits_checker,
    double bin_min_visits);

void at_driver_langevin_integrator__finish(
    at_driver_langevin_integrator_t *intgr);

double at_driver_langevin_integrator__integrate(
    at_driver_langevin_integrator_t *intgr,
    double beta_from, double beta_to);

double at_driver_langevin_integrator__integrate_with_visits_checker(
    at_driver_langevin_integrator_t *intgr,
    double beta_from, double beta_to,
    at_bool_t *success);


#endif
