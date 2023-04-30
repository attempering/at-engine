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

/* Simplest program of using the at module */
#ifdef HEADER_ONLY
#include "at-engine__src.h"
#else
#include "at-engine.h"
#endif

int main(void)
{
  at_llong_t step = 0, nsteps = 500000;

  at_t* at = at__open("at.cfg", AT__FALSE, AT__TRUE, NULL, AT__FALSE);

  //at_driver_langevin_rng__reset(at->driver->langevin->rng, 1234);

  for (step = 1; step <= nsteps; step++) {
    at->energy = 0.0;
    at__step(at, step, NULL);
  }

  at__close(at);

  return 0;
}
