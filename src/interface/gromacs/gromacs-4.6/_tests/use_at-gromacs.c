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

#include "../src/kernel/at-gromacs/at-gromacs.h"

int main(void)
{
  at_llong_t step = 0, nsteps = 1;
  atgmx_t atgmx[1];
  t_commrec cr[1];
  t_inputrec ir[1];
  at_bool_t from_cpt = AT__FALSE;

  atgmx__init(atgmx, "at.cfg", ir, cr, from_cpt, AT__INIT_VERBOSE);

  atgmx__finish(atgmx);

  return 0;
}
