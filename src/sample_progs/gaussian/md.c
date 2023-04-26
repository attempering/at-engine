#define GAUSSIAN_USE_MDSYS_ALIASES__
#include "veritools/models/gaussian-energy/gaussian.h"



typedef long long llong_t;
double boltz = 1;



void run_md(mdsys_t* mdsys, double beta, llong_t nsteps)
{
  llong_t step = 0;

  for (step = 1; step <= nsteps; step++) {
    mdsys_step(mdsys, beta);
    mdsys_add_to_histograms(mdsys);
    //fprintf(stderr, "%lld %g\n", step, mdsys->epot);
  }

  mdsys_save_histograms(mdsys);
}



int main(void)
{
  double sigma = 10.0; /* spring constant */
  double epot_dt = 0.05; /* time step */
  double beta = 1.0;
  llong_t nsteps = 100000;

  mdsys_t* mdsys;

  mdsys = mdsys_new(sigma, epot_dt, beta, beta, boltz);

  run_md(mdsys, beta, nsteps);

  mdsys_delete(mdsys);

  return 0;
}
