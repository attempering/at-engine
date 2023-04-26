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



static int test_fill_range(mb_t *mb)
{
  double *raw_data = mb_mock_sm_moments(mb, 0.2);

  mb_zerofiller_t *zf = mb->zerofiller;

  int i;

  int passed = 1;

  mb_zerofiller_fill_range_with_proper_sums(mb->zerofiller, 0, mb->n - 1);

  for (i = 0; i < mb->n; i++) {
    fprintf(stderr, "%d: %g %g\n", i, raw_data[i], zf->vals[i]);
    if (zf->vals[i] == 0.0) {
      passed = 0;
    }
  }

  free(raw_data);

  return passed;
}




int main(void)
{
  mb_t mb[1];
  int passed;

  init_mb_object(mb);

  passed = test_fill_range(mb);

  mb__finish(mb);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
