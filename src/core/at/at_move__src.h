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


#include "at_move.h"

#include "../zcom/zcom.h"
#include "utils/at_utils.h"



int at__move(at_t *at, at_llong_t step, at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush)
{
  double invwf = 1.0, temp1, temp2, Eav = 0.0, neg_dlnwf_dbeta;
  int ib, rep;

  temp1 = at__beta_to_temp(at, at->beta);

  ib = at_mb__beta_to_index(at->mb, at->beta, 1);

  /* update energy data, change at->beta */
  /* repeat several times to change the temperature */
  for (rep = 0; rep < at->driver->move_repeats; rep++) {
    /* 1. deposit the current energy and temperature */
    at_mb__add(at->mb, at->energy, at->beta, &ib, &invwf, &neg_dlnwf_dbeta);

    /* 2. add the current data point to the energy histogram */
    at_eh__add(at->eh, ib, at->energy);

    /* 3. use the Langevin equation to update the temperature */
    at->beta = at_driver_langevin__move(at->driver->langevin, at->mb, at->energy, at->beta,
        ib, invwf, neg_dlnwf_dbeta, &Eav);
  }

  temp2 = at__beta_to_temp(at, at->beta);

  if (at__do_every(step, at->mb->nstrefresh, 0, blast)) {
    at_mb__refresh_et(at->mb, 1);
  }

  at__output(at, step, ib, invwf, temp1, temp2, Eav, bfirst, blast, btr, bflush);

  return 0;
}


#endif
