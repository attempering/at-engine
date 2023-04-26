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
llong_t nsteps = 500000;

double epot = -100.0;


void run_cst_md(at_t* at, llong_t nsteps)
{
  llong_t step = 0;

  zcom_mtrng__init_from_seed(at->mtrng, langevin_seed);

  //fprintf(stderr, "0 %g %g | %u %d | %g %g\n", at->beta, at->Ea, at->mtrng->arr[0], at->mtrng->index, mdsys->x, mdsys->v);

  for (step = 1; step <= nsteps; step++) {
    at_bool_t is_first_step = (step == 1);
    at_bool_t is_last_step = (step == nsteps);
    at_bool_t is_tempering_step = (at->nsttemp > 0 && (step % at->nsttemp == 0)) || (at->nsttemp <= 0);
    at_bool_t flush_output = FALSE;

    //fprintf(stderr, "%lld %g %g | %u %d\n", step, at->beta, at->Ea, at->mtrng->arr[0], at->mtrng->index);

    if (is_tempering_step) {
      at->Ea = epot;

      at_move(at, step, is_first_step, is_last_step, TRUE, flush_output);
      at->beta = at->mb->beta;

      //fprintf(stderr, "%lld %g %g | %u %d\n", step, at->beta, at->Ea, at->mtrng->arr[0], at->mtrng->index);
      //getchar();
    }

  }

}


#ifdef ENABLE_MINICST
void run_minicst_md(at_t* at, llong_t nsteps)
{
  llong_t step = 0;
  minicst_t* minicst = minicst_new(at->bmin, at->bmax, at->mb->n, at->langevin->dt, at->langevin->dTmax, langevin_seed);

  //fprintf(stderr, "0 %g %g | %u %d\n", at->beta, at->Ea, minicst->langevin->mtrng->arr[0], minicst->langevin->mtrng->index);

  for (step = 1; step <= nsteps; step++) {
    at_bool_t btr = (at->nsttemp > 0 && (step % at->nsttemp == 0)) || (at->nsttemp <= 0);

    //fprintf(stderr, "%lld %g %g | %u %d\n", step, at->beta, at->Ea, minicst->langevin->mtrng->arr[0], minicst->langevin->mtrng->index);

    if (btr) {
      at->Ea = epot;
      minicst_add(minicst, at->beta, epot);
      minicst_move(minicst, &at->beta, epot);
      //fprintf(stderr, "%lld %g %g | %u %d\n", step, at->beta, at->Ea, minicst->langevin->mtrng->arr[0], minicst->langevin->mtrng->index);
      //getchar();
    }
  }

  minicst_save(minicst, "cst.dat");
  minicst_delete(minicst);
}
#endif



int main(int argc, char** argv)
{
  int suffix = 0; /* sequence ID for multiple runs */
  const char* fn_cfg = "at.cfg";

  if (argc > 1) {
    fn_cfg = argv[1];
    fprintf(stderr, "reading configuration file %s\n", fn_cfg);
  }

  //remove("TRACE0");
  remove("atdata0/trace.dat");

  at_t* at = at_open(fn_cfg, FALSE, TRUE, boltz, epot_dt, suffix);
  at_manifest(at, "atdata0/at-manifest.dat", 3);

  if (use_minicst) { // simplified CST for reference

#ifdef ENABLE_MINICST
    run_minicst_md(at, nsteps);
#else
    zcom_util__fatal("Please define ENABLE_MINICST and recompile the program\n");
#endif

  } else {
    run_cst_md(at, nsteps);
  }

  fprintf(stderr, "CST simulation finished successfully.\n");

  at_close(at);

  return 0;
}
