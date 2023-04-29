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

#ifndef AT_DRIVER_LANGEVIN_INTEGRATOR__DEF_H__
#define AT_DRIVER_LANGEVIN_INTEGRATOR__DEF_H__

typedef struct at_driver_langevin_zerofiller_t_ at_driver_langevin_zerofiller_t;

#include "../../../mb/accum/at_mb_accum__def.h"

/* parameters for the integrator */
typedef struct at_driver_langevin_integrator_t_ {
  int n; /* same as distr->domain->n */
  double    beta_min; /* same as distr->domain->bmin */
  double    beta_max; /* same as distr->domain->bmax */
  double    beta_del; /* same as distr->domain->bdel */

  int       sgn;
  double    beta_begin;
  double    beta_end;
  int       ib_begin;
  int       ib_end;
  double    dbeta_left;
  double    dbeta_right;

  double    *vals; /* values without zerofiller */

  int use_zerofiller;

  /* reference to the zero-filler */
  at_driver_langevin_zerofiller_t zerofiller[1];

  at_mb_t *mb; /* reference */

} at_driver_langevin_integrator_t;

#endif
