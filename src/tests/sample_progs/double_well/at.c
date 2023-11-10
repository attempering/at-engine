#include "at-engine__src.h"

#include "veritools/models/double_well/double_well.h"
#include "veritools/utilities/histogram/histogram.h"


long nsteps = 1000000;

double sigma = 0.2;


static void run(at_t *at)
{
  at_params_step_t step_params[1];
  long t;
  double_well_t dw[1];
  double_well_params_t dw_params0[1], dw_params[1];
  double bias_c = 0.0;
  double beta_ref_temp = at__temp_to_beta(at, at->distr->bias->ref_temp);
  double beta_max_temp = at__temp_to_beta(at, at->distr->bias->max_temp);
  double beta_tol = at->distr->domain->beta_del * 2;
  histogram_t hist_ref_temp[1], hist_max_temp[1];

  at_params_step__init(step_params);

  double_well__init(dw, NULL);
  double_well_params__init_default(dw_params0);
  double_well_params__init_default(dw_params);

  histogram__init(hist_ref_temp, -5, 5, 0.01);
  histogram__init(hist_max_temp, -5, 5, 0.01);

  for (t = 1; t <= nsteps; t++) {
    double beta = at__get_beta(at);

    double w_comb = at__get_bias_w_comb(at),
           w_deriv = at__get_bias_w_deriv(at);
    
    double epot0, bias;

    if (fabs(beta - beta_ref_temp) < beta_tol) {
      histogram__add(hist_ref_temp, dw->x);
    }

    if (fabs(beta - beta_max_temp) < beta_tol) {
      histogram__add(hist_max_temp, dw->x);
    }

    // form the biased potential
    dw_params->c = w_comb * bias_c;
    double_well__metro_params(dw, dw_params, beta, sigma);

    // compute the effective energy under the bias
    epot0 = double_well_params__energy(dw_params0, dw->x);
    bias = w_deriv * bias_c * dw->x;

    at__set_energy(at, epot0 + bias);

    step_params->step = t;
    step_params->is_first_step = (t == 1);
    step_params->is_final_step = (t == nsteps);

    at__move(at, step_params);
  }

  double_well__finish(dw);

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
