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

#ifndef AT_DRIVER_LANGEVIN_MOVE_UTILS__SRC_H__
#define AT_DRIVER_LANGEVIN_MOVE_UTILS__SRC_H__



#include "at_driver_langevin_move_utils.h"



double at_driver_langevin_move__get_acceptance_ratio(
    const at_driver_langevin_t *langevin)
{
  if (langevin->total > 0) {
    return 1 - langevin->rejects/langevin->total; 
  } else {
    return 0;
  }
}



void at_driver_langevin_move__print_acceptance_ratio_suggestions(
    const at_driver_langevin_t *langevin)
{
  double ar = at_driver_langevin_move__get_acceptance_ratio(langevin);
  const double ar_max = 0.98;
  const double ar_min = 0.50;

  if (ar > ar_max) {
    fprintf(stderr, "\rInfo@at.driver.langevin.move: acceptance ratio %g, consider increasing Tdt\n", ar);
  } else if (ar < ar_min) {
    fprintf(stderr, "\rInfo@at.driver.langevin.move: acceptance ratio %g, consider decreasing Tdt\n", ar);
  }

}



#endif
