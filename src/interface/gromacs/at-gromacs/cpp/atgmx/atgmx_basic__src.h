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

#ifndef ATGMX_BASIC__SRC_H__
#define ATGMX_BASIC__SRC_H__

#include "atgmx__def.h"
//#include "atgmx_mpi.h"


void AtGmx::update_thermostat_temperatures(t_inputrec *ir) const
{
  int i;

  for (i = 0; i < ir->opts.ngtc; i++) {
    ir->opts.ref_t[i] = (real) at->utils->temp_thermostat;
  }
}



AtGmx::AtGmx(
    const char *fn_cfg,
    t_inputrec *ir,
    t_commrec *cr,
    at_bool_t is_continuation,
    at_bool_t multi_sims,
    at_flags_t flags)
{
  if (fn_cfg != NULL) {
    enabled = AT__TRUE;
  } else {
    enabled = AT__FALSE;
  }

  is_master = (MASTER(cr) ? AT__TRUE : AT__FALSE);

  if (is_master) {
    at_params_sys_t sys_params[1];

    sys_params->boltz = BOLTZ;
    sys_params->id = 0;
    sys_params->md_time_step = ir->delta_t;
    sys_params->multi_sims = multi_sims;
    sys_params->is_continuation = is_continuation;

    at__init(at, fn_cfg, sys_params, flags);
  }

#ifdef GMX_MPI
  /* tell every node the settings on the master
   * valid only for PP only node, maybe we need to
   * consider using mpi_comm_mysim for more advanced versions
   * we pass MPI_COMM_NULL to avoid the case of one-node-mpi */
  if (PAR(cr)) {
    init_mpi(PAR(cr) ? cr->mpi_comm_mygroup : MPI_COMM_NULL);
  }
#endif

  update_thermostat_temperatures(ir);

  update_force_scale(cr);

  if (is_master) {
    at__manifest(at);
  }
}



AtGmx::~AtGmx()
{
  if (enabled && is_master) {
    at__finish(at, AT__FINISH_CLOSE_CFG);
  }
}


#endif
