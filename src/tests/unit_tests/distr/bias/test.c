#include "at-engine__src.h"

static void run() {
  at_t at[1];
  int i;

  at__init(at, "at.cfg", NULL, 0);

  for (i = 0; i < at->distr->domain->n; i++) {
    double beta = at->distr->domain->beta_min + (i + 0.5) * at->distr->domain->beta_del;

    double w_comb = at_distr__get_bias_w_comb(at->distr, beta);
    double w_deriv = at_distr__get_bias_w_deriv(at->distr, beta);

    printf("%g %g %g\n", beta, w_comb, w_deriv);
  }

  at__finish(at);
}

int main(void)
{
  run();
  return 0;
}
