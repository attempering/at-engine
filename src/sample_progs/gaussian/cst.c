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

//#define ENABLE_MINICST 1

#include "adaptive_tempering__src.h"


#ifdef ENABLE_MINICST
#define ZCOM_RNG__ 1
#include "veritools/algorithms/minicst/minicst.h"
#endif


typedef long long llong_t;
double boltz = 1.0;
int use_minicst = 0;
uint32_t langevin_seed = 1234;

double sigma = 10.0; /* standard deviation of energy distributions */
double epot_dt = 0.01; /* equilibration rate, larger value means the system is able to equilibrate faster */
llong_t nsteps = 5000000;


void run_cst_md(at_t* at, mdsys_t* mdsys, llong_t nsteps)
{
  llong_t step = 0;

  zcom_mtrng__init_from_seed(at->mtrng, langevin_seed);

  //fprintf(stderr, "0 %g %g | %u %d | %g %g\n", at->beta, at->Ea, at->mtrng->arr[0], at->mtrng->index, mdsys->x, mdsys->v);

  for (step = 1; step <= nsteps; step++) {
    at_bool_t is_first_step = (step == 1);
    at_bool_t is_last_step = (step == nsteps);
    at_bool_t is_tempering_step = (at->nsttemp > 0 && (step % at->nsttemp == 0)) || (at->nsttemp <= 0);
    at_bool_t flush_output = FALSE;

    mdsys__step(mdsys, at->beta);

    //fprintf(stderr, "%lld %g %g | %u %d | %g %g\n", step, at->beta, at->Ea, at->mtrng->arr[0], at->mtrng->index, mdsys->x, mdsys->v);

    if (is_tempering_step) {
      at->energy = mdsys->epot;

      at__move(at, step, is_first_step, is_last_step, TRUE, flush_output);

      //fprintf(stderr, "%lld %g %g | %u %d | %g %g\n", step, at->beta, at->Ea, at->mtrng->arr[0], at->mtrng->index, mdsys->x, mdsys->v);
      //getchar();

      //at_mb__write(at->mb, at->langevin);
      //exit(1);
    }

  }

}


#ifdef ENABLE_MINICST
void run_minicst_md(at_t* at, mdsys_t* mdsys, llong_t nsteps)
{
  llong_t step = 0;
  minicst_t* minicst = minicst__new(at->mb->bmin, at->mb->bmax, at->mb->n, at->langevin->dt, at->langevin->dTmax, langevin_seed);

  //fprintf(stderr, "0 %g %g | %u %d | %g %g\n", at->beta, at->Ea, minicst->langevin->mtrng->arr[0], minicst->langevin->mtrng->index, mdsys->x, mdsys->v);

  for (step = 1; step <= nsteps; step++) {
    at_bool_t btr = (at->nsttemp > 0 && (step % at->nsttemp == 0)) || (at->nsttemp <= 0);

    mdsys__step(mdsys, at->beta);

    //fprintf(stderr, "%lld %g %g | %u %d | %g %g\n", step, at->beta, at->Ea, minicst->langevin->mtrng->arr[0], minicst->langevin->mtrng->index, mdsys->x, mdsys->v);

    if (btr) {
      at->energy = mdsys->epot;
      minicst__add(minicst, at->beta, mdsys->epot);
      minicst__move(minicst, &at->beta, mdsys->epot);
      //fprintf(stderr, "%lld %g %g | %u %d | %g %g\n", step, at->beta, at->Ea, minicst->langevin->mtrng->arr[0], minicst->langevin->mtrng->index, mdsys->x, mdsys->v);
      //getchar();
    }
  }

  minicst__save(minicst, "cst.dat");
  minicst__delete(minicst);
}
#endif



int main(int argc, char** argv)
{
  int suffix = 0; /* sequence ID for multiple runs */
  const char* fn_cfg = "at.cfg";
  mdsys_t* mdsys;

  if (argc > 1) {
    fn_cfg = argv[1];
    fprintf(stderr, "reading configuration file %s\n", fn_cfg);
  }

  //remove("TRACE0");
  remove("atdata0/trace.dat");

  at_t* at = at__open(fn_cfg, FALSE, TRUE, boltz, epot_dt, suffix);
  //at__manifest(at, "atdata0/at-manifest.dat", 3);

  mdsys = mdsys__new(sigma, epot_dt, at->mb->bmin, at->mb->bmax, boltz);

  if (use_minicst) { // simplified CST for reference
#ifdef ENABLE_MINICST
    run_minicst_md(at, mdsys, nsteps);
#else
    zcom_util__fatal("Please define ENABLE_MINICST and recompile the program\n");
#endif
  } else {
    run_cst_md(at, mdsys, nsteps);
  }

  fprintf(stderr, "CST simulation finished successfully.\n");

  mdsys__delete(mdsys);

  at__close(at);

  return 0;
}
