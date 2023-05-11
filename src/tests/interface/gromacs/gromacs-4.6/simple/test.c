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

#include "at-gromacs/at-gromacs__src.h"



void init_gromacs_vars(t_commrec *cr, t_inputrec *ir, gmx_enerdata_t *enerd)
{
  cr->nodeid = 0;
  cr->nnodes = 1;

  cr->ms = NULL;

  ir->delta_t = 0.002;

  ir->opts.ngtc = 1;
  ir->opts.ref_t = calloc(1, sizeof(real));
  ir->opts.ref_t[0] = 300;

  enerd->term[F_EPOT] = 0.0;
}



int main(void)
{
  at_llong_t step = 0, nsteps = 10;
  atgmx_t atgmx[1];
  t_commrec cr[1];
  t_inputrec ir[1];
  gmx_enerdata_t enerd[1];
  at_bool_t from_cpt = AT__FALSE;

  // initialize GROMACS variables
  init_gromacs_vars(cr, ir, enerd);

  atgmx__init(atgmx, "at.cfg", ir, cr, from_cpt, AT__INIT_VERBOSE);

  for (step = 0; step <= nsteps; step++) {
    at_bool_t is_first_step = (step == 0);
    at_bool_t is_last_step = (step == nsteps);
    at_bool_t has_global_stats = AT__TRUE;
    at_bool_t is_xtc_step = AT__TRUE;
    at_bool_t is_ns_step = AT__TRUE;

    enerd->term[F_EPOT] = -10000.0;

    atgmx__move(atgmx, enerd, step,
        is_first_step, is_last_step, has_global_stats,
        is_xtc_step, is_ns_step,
        cr);
  }

  atgmx__finish(atgmx);

  free(ir->opts.ref_t);

  return 0;
}
