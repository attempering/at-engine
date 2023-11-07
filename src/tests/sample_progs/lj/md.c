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

#include "veritools/models/lj/lj.h"
#include "veritools/models/lj/ljeos.h"


/* basic molecular dynamics simulation in the NVT ensemble */

int n = 108;
int nequil = 10000;
int nsteps = 100000;
double rho = 0.7;
double tp = 1.5;
double rc_def = 2.5;
double dt = 0.002;
double thermostat_damp = 10.0;


int main(void)
{
  int t;
  lj_t *lj;
  double ep_sum = 0, ek_sum = 0, pres_sum = 0;

  lj = lj__open(n, rho, rc_def);

  /* equilibration */
  for (t = 1; t <= nequil; t++) {
    lj__vv(lj, dt);
    lj__vel_rescale(lj, tp, dt * thermostat_damp);
    if ( t % 1000 == 0 ) {
      printf("%d, U %g, P %g\n", t, lj->epot, lj__calc_pres(lj, tp));
    }
  }

  /* production */
  for (t = 1; t <= nsteps; t++ ) {
    lj__vv(lj, dt);
    lj->ekin = lj__vel_rescale(lj, tp, dt * thermostat_damp);

    ep_sum += lj->epot;
    ek_sum += lj->ekin;
    pres_sum += lj__calc_pres(lj, tp);
  }

  /* print statistics */
  {
    double u_ref, pres_ref;
    u_ref = ljeos3d__get(rho, tp, &pres_ref, NULL, NULL);
    printf("rho %g, tp %g(%g), "
           "U/N %g (ref. %g), "
           "P %g (ref. %g)\n",
        rho, tp, 2*ek_sum/nsteps/lj->dof,
        ep_sum/nsteps/n, u_ref,
        pres_sum/nsteps, pres_ref);
  }

  lj__close(lj);

  return 0;
}

