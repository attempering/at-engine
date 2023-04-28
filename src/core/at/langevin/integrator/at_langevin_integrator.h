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

#ifndef AT_LANGEVIN_INTEGRATOR_H__
#define AT_LANGEVIN_INTEGRATOR_H__



/* integrator of the estimated averager over a temperature range */



#include "../../mb/at_mb__def.h"


int at_langevin_integrator__init(at_langevin_integrator_t *intgr,
    at_mb_t *mb, int use_zerofiller);

void at_langevin_integrator__finish(at_langevin_integrator_t *intgr);

double at_langevin_integrator__integrate(at_langevin_integrator_t *intgr,
    double beta_from, double beta_to);


#endif
