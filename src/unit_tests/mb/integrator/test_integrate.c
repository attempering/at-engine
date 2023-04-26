/* testing the effect of the window boundary convention */


//#define MB_DBG__ 1
#include "at/at__src.h"



double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


long ntimes = 100000;



void init_mb_object(mb_t *mb)
{
  cfg_t *cfg = cfg_open("at.cfg");

  // beta_min and beta_max are to be read from the configuration file
  mb__cfg_init(mb, cfg, boltz, 0.0, 0.0, NULL, 1);

  cfg_close(cfg);
}



double *mb_mock_sm_moments(mb_t *mb, double fill_prob)
{
  int i;
  sm_t *sm;
  mtrng_t rng[1];
  double *data;

  mtrng_init_from_seed(rng, 12345);

  data = (double *) calloc(mb->n, sizeof(double));

  for (i = 0; i < mb->n; i++) {

    if (mtrng_rand01(rng) < fill_prob) {

      sm = mb_accum__get_proper_sums(mb->accum, i, i);
      double beta = mb->bmin + (i + 0.5) * mb->bdel;
      double epot = -beta * (gaussian_sigma * gaussian_sigma);

      sm->s = 1.0;
      sm->se = epot;

      data[i] = epot;

    } else {

      data[i] = 0.0;

    }

  }

  return data;
}



static double get_exact_integral(double beta1, double beta2)
{
  // E = -beta * sigma^2
  return -gaussian_sigma * gaussian_sigma * 0.5 * (beta2 * beta2 - beta1 * beta1);
}



static int test_integrate(mb_t *mb, double fill_prob)
{
  double *raw_data = mb_mock_sm_moments(mb, fill_prob);
  int passed;

  mb_integrator_t *intgr = mb->integrator;
  mb_zerofiller_t *zf = mb->zerofiller;

  double beta1 = mb->bmin * 0.7 + mb->bmax * 0.3;
  double beta2 = mb->bmin * 0.2 + mb->bmax * 0.8;
  int i, ib1, ib2;

  double integral = mb_integrator__integrate(intgr, beta1, beta2);
  double exact_integral = get_exact_integral(beta1, beta2);

  ib1 = intgr->ib_begin;
  ib2 = intgr->ib_end;

  mb_zerofiller_fill_range_with_proper_sums(zf, ib1, ib2);

  for (i = ib1; i <= ib2; i++) {
    fprintf(stderr, "%d: %g %g\n", i, raw_data[i], zf->vals[i]);
  }

  fprintf(stderr, "integral %g vs %g\n", integral, exact_integral);

  free(raw_data);

  passed = (fabs(integral - exact_integral) < 5.0);

  return passed;
}




int main(int argc, char **argv)
{
  mb_t mb[1];
  double fill_prob = 0.5;
  int passed;

  init_mb_object(mb);

  if (argc > 1) {
    fill_prob = atof(argv[1]);
  }

  passed = test_integrate(mb, fill_prob);

  mb__finish(mb);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
