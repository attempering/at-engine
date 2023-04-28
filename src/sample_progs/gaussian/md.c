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

#define GAUSSIAN__USE_MDSYS_ALIASES__
#include "veritools/models/gaussian-energy/gaussian.h"



typedef long long llong_t;
double boltz = 1;



void run_md(mdsys_t* mdsys, double beta, llong_t nsteps)
{
  llong_t step = 0;

  for (step = 1; step <= nsteps; step++) {
    mdsys__step(mdsys, beta);
    mdsys__add_to_histograms(mdsys);
    //fprintf(stderr, "%lld %g\n", step, mdsys->epot);
  }

  mdsys__save_histograms(mdsys);
}



int main(void)
{
  double sigma = 10.0; /* spring constant */
  double epot_dt = 0.05; /* time step */
  double beta = 1.0;
  llong_t nsteps = 100000;

  mdsys_t* mdsys;

  mdsys = mdsys__new(sigma, epot_dt, beta, beta, boltz);

  run_md(mdsys, beta, nsteps);

  mdsys__delete(mdsys);

  return 0;
}
