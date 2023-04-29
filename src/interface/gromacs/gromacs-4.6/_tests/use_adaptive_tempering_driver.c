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

#include "../src/kernel/adaptive_tempering_driver.h"

int main(void)
{
  at_llong_t step = 0, nsteps = 500000;

  at_t* at = at__open("at.cfg", FALSE, TRUE, 1.0, 0.01, 0);

  at->energy = 0.0;

  for (step = 1; step <= nsteps; step++) {
    at__move(at, step, (step == 1), (step == nsteps), TRUE, FALSE);
  }

  at__close(at);

  return 0;
}
