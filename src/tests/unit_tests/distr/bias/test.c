#include "at-engine__src.h"

static void run() {
  at_t at[1];
  int i;
  const double tol = 1e-5;
  int err = 0;

  at__init(at, "at.cfg", NULL, 0);

  for (i = 0; i < at->distr->domain->n; i++) {
    double beta = at->distr->domain->beta_min + (i + 0.5) * at->distr->domain->beta_del;

    double w_comb = at_distr__get_bias_w_comb(at->distr, beta);
    double w_deriv = at_distr__get_bias_w_deriv(at->distr, beta);

    printf("%g %g %g\n", beta, w_comb, w_deriv);

    {
      double temp = at__beta_to_temp(at, beta);
      double w_comb2 = (temp - at->distr->bias->ref_temp)
                     / (at->distr->bias->max_temp - at->distr->bias->ref_temp);
      double w_deriv2 = - at->distr->bias->ref_temp
                     / (at->distr->bias->max_temp - at->distr->bias->ref_temp);

      if (fabs(w_comb - w_comb2) > tol) {
        err += 1;
      }

      if (fabs(w_deriv - w_deriv2) > tol) {
        err += 2;
      }
    }
  }

  if (err == 0) {
    fprintf(stderr, "Passed.\n");
  } else {
    fprintf(stderr, "Failed.\n");
  }

  at__finish(at);
}

int main(void)
{
  run();
  return 0;
}
