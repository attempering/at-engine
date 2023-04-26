/* testing the effect of the window boundary convention */


//#define MB_DBG__ 1
#include "at/at__src.h"



double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


long ntimes = 100000;



void init_mb_and_langevin(at_mb_t *mb, at_langevin_t *langevin)
{
  zcom_cfg_t *cfg = zcom_cfg__open("at.cfg");

  // beta_min and beta_max are to be read from the configuration file
  at_mb__cfg_init(mb, cfg, boltz, 0.0, 0.0, NULL, 1);

  at_langevin__cfg_init(langevin, mb, cfg, 1);

  zcom_cfg__close(cfg);
}



double *mb_mock_sm_moments(at_mb_t *mb, double fill_prob)
{
  int i;
  at_mb_sm_t *sm;
  zcom_mtrng_t rng[1];
  double *data;

  zcom_mtrng__init_from_seed(rng, 12345);

  data = (double *) calloc(mb->n, sizeof(double));

  for (i = 0; i < mb->n; i++) {

    if (zcom_mtrng__rand01(rng) < fill_prob) {

      sm = at_mb_accum__get_proper_sums(mb->accum, i, i);
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



static int test_fill_range(at_mb_t *mb, at_langevin_t *langevin)
{
  double *raw_data = mb_mock_sm_moments(mb, 0.2);

  at_langevin_zerofiller_t zf[1];

  int i;

  int passed = 1;

  at_langevin_zerofiller__init(zf, mb);

  at_langevin_zerofiller__fill_range_with_proper_sums(zf, 0, mb->n - 1);

  for (i = 0; i < mb->n; i++) {
    fprintf(stderr, "%d: %g %g\n", i, raw_data[i], zf->vals[i]);
    if (zf->vals[i] == 0.0) {
      passed = 0;
    }
  }

  at_langevin_zerofiller__finish(zf);

  free(raw_data);

  return passed;
}




int main(void)
{
  at_mb_t mb[1];
  at_langevin_t langevin[1];
  int passed;

  init_mb_and_langevin(mb, langevin);

  passed = test_fill_range(mb, langevin);

  at_mb__finish(mb);
  at_langevin__finish(langevin);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
