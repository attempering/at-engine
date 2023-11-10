#include "at-engine__src.h"

#include "veritools/utilities/rng/rng.h"
#include "veritools/utilities/histogram/histogram.h"


long nsteps = 1000000;


at_bool_t use_metro = AT__TRUE;
double sigma = 0.2;


static void run(at_t *at)
{
  at_params_step_t step_params[1];
  long t;
  double x;
  rng_t rng[1];
  double bias_c = 1.0;
  double beta_ref_temp = at__temp_to_beta(at, at->distr->bias->ref_temp);
  double beta_max_temp = at__temp_to_beta(at, at->distr->bias->max_temp);
  double beta_tol = at->distr->domain->beta_del * 2;
  histogram_t hist_ref_temp[1], hist_max_temp[1];

  at_params_step__init(step_params);

  rng__init(rng, 123);
  x = rng__rand_normal(rng) * sqrt(1.0/beta_ref_temp);

  histogram__init(hist_ref_temp, -5, 5, 0.05);
  histogram__init(hist_max_temp, -5, 5, 0.05);

  for (t = 1; t <= nsteps; t++) {
    double beta = at__get_beta(at);

    double w_comb = at__get_bias_w_comb(at),
           w_deriv = at__get_bias_w_deriv(at);

    double epot0, bias;

    if (fabs(beta - beta_ref_temp) < beta_tol) {
      histogram__add(hist_ref_temp, x);
    }

    if (fabs(beta - beta_max_temp) < beta_tol) {
      histogram__add(hist_max_temp, x);
    }

    // the biased potential is
    //
    // V(x) = 0.5 x^2 + w_comb * bias_c * x
    //      = 0.5 (x + w_comb * bias_c)^2 - 0.5 * (w_comb * bias_c)^2
    //
    if (use_metro) {
      double epot_old = 0.5*x*x + w_comb * bias_c * x;
      double x_new = x + sigma * (rng__rand_01(rng)*2 - 1);
      double epot_new = 0.5*x_new*x_new + w_comb * bias_c * x_new;
      double epot_del = epot_new - epot_old;

      if (epot_del < 0 || rng__rand_01(rng) < exp(-beta*epot_del)) {
        x = x_new;
      }
    } else {
      x = -w_comb * bias_c + rng__rand_normal(rng) / sqrt(beta);
    }

    // compute the effective energy under the bias
    epot0 = 0.5 * x * x;
    bias = w_deriv * bias_c * x;

    at__set_energy(at, epot0 + bias);

    step_params->step = t;
    step_params->is_first_step = (t == 1);
    step_params->is_final_step = (t == nsteps);

    at__move(at, step_params);
  }

  rng__finish(rng);

  histogram__save(hist_ref_temp, "xhist_Tref.dat");
  histogram__finish(hist_ref_temp);

  histogram__save(hist_max_temp, "xhist_Tmax.dat");
  histogram__finish(hist_max_temp);
}


int main(void)
{
  at_t at[1];

  at__init(at, "at.cfg", NULL, AT__INIT_VERBOSE);

  run(at);

  at__finish(at);

  return 0;
}
