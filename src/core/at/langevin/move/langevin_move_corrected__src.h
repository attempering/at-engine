#ifndef AT__LANGEVIN_MOVE_CORRECTED__SRC_H__
#define AT__LANGEVIN_MOVE_CORRECTED__SRC_H__



#include "langevin_move_basic.h"



const int use_cheap_av_energy_for_backward_move = 0;
const int use_cheap_av_energy_for_forward_move = 0;
const int apply_dkt_max = 0;



// calculate the factor of
//
//     p (kT_new)
// ln ------------
//     p (kT_old)
//
//         p(beta_new) | d beta_new /d(kT_new) |
//  = ln -----------------------------------------
//         p(beta_old) | d beta_old /d(kT_old) |
//
//         p(beta_new) beta_new^2
//  = ln --------------------------
//         p(beta_old) beta_old^2
//

static double langevin_move__calc_lnp_ratio(
    langevin_t *langevin,
    mb_t *mb,
    double beta_old, double beta_new,
    double current_energy,
    double invwf_old, double invwf_new)
{
  double lnp_ratio;
  double ln_invwf_ratio;
  double ln_beta_kt_jacob;
  double delta_beta_energy;
  double energy_beta_integral;

  // ln(wf(beta_new)/wf(beta_old))
  ln_invwf_ratio = log(invwf_old / invwf_new);

  // ln(beta_new^2/beta_old^2)
  ln_beta_kt_jacob = 2 * log(beta_new/beta_old);

  // (beta_old - beta_new) current_energy
  delta_beta_energy = (beta_old - beta_new) * current_energy;

  // Int {beta_old to beta_new} <U> d beta
  // = -ln Z(beta_new) + ln Z(beta_old)
  if (langevin->integrate_func) {
    // use the custom integrator
    energy_beta_integral = (*langevin->integrate_func)(mb, beta_old, beta_new);
  } else {
    energy_beta_integral = mb_integrator__integrate(mb->integrator, beta_old, beta_new);
  }

  lnp_ratio = ln_invwf_ratio
            + ln_beta_kt_jacob
            + delta_beta_energy
            + energy_beta_integral;

  if (langevin_move__debug__ >= 2) {
    fprintf(stderr, "langevin_move__calc_lnp_ratio():\n");
    fprintf(stderr, "  beta %g => %g\n", beta_old, beta_new);
    fprintf(stderr, "  log(invwf) %g, %g => %g\n", ln_invwf_ratio, invwf_old, invwf_new);
    fprintf(stderr, "  log(beta_kt_jacob) %g\n", ln_beta_kt_jacob);
    fprintf(stderr, "  (beta_old - beta_new)*energy %g\n", delta_beta_energy);
    fprintf(stderr, "  Int energy dbeta %g\n", energy_beta_integral);
    fprintf(stderr, "  ln(p_ratio): %g\n", lnp_ratio);
    if (langevin_move__debug__ >= 3) {
      getchar();
    }
  }

  return lnp_ratio;
}



// calculate the logarithm of ratio of
// the two generation probabilities
//
//     g (kT_new -> kT_old)   <-- (virtual backward move)
// ln ----------------------
//     g (kT_old -> kT_new)   <-- (forward move)
//
//
// g(kT -> kT')
//
//          1                   [kT' - (kT + dkT_deterministic)]^2
// = ------------------ exp{ - ------------------------------------- }
//    sqrt(4 Pi dt) kT                     4 (kT)^2 dt
//
static double langevin_move__calc_lng_ratio(
    langevin_t *langevin,
    mb_t *mb,
    double kt_old, double kt_new,
    double current_energy,
    double kt_new_expected,
    int ib_new,
    double neg_dlnwf_dbeta_new,
    double time_step)
{
  double lng_ratio = 0;
  double dkt_new, kt_old_expected;
  double dkt_new_deterministic;
  double bin_av_energy_new;
  double dkt_old;
  double ln_jac;
  double forward, backward;

  // Jacobian: ln(T_old/T_new)
  // due to the coefficient of the Gaussian random noise
  ln_jac = log(kt_old/kt_new);

  // forward move
  // [kT_new - (kT_old + dkT_old_deterministic)]^2 / (2 kT_old)^2
  dkt_new = (kt_new - kt_new_expected)/(kt_old*2);
  // This factor is on the denominator,
  // but with a negative sign exp(-dkt_new^2/dt)
  // we add it to the lng_ratio.
  forward = dkt_new * dkt_new / time_step;

  // virtual backward move
  // [kT_old - (kT_new + dkT_new_deterministic)]^2 / (2 kT_new)^2
  // dkt_new = a(kT_new) * time_step;
  dkt_new_deterministic = langevin_move__calc_dkt_deterministic(
      langevin,
      mb,
      ib_new,
      time_step,
      neg_dlnwf_dbeta_new,
      current_energy,
      &bin_av_energy_new,
      use_cheap_av_energy_for_backward_move);

  kt_old_expected = kt_new + dkt_new_deterministic;

  dkt_old = (kt_old - kt_old_expected)/(kt_new*2);

  // This factor is on the numerator,
  // but with a negative sign exp(-dkt_new^2/dt)
  // we subtract it to the lng_ratio.
  backward = dkt_old * dkt_old / time_step; 

  lng_ratio = ln_jac + forward - backward;

  if (langevin_move__debug__ >= 2) {
    fprintf(stderr, "langevin_move__calc_lng_ratio():\n");
    fprintf(stderr, "  kT %g => %g, time step %g\n", kt_old, kt_new, time_step);
    fprintf(stderr, "  ln_jac %g\n", ln_jac);
    fprintf(stderr, "  forward %g, dkt_new %g, kt_new %g, expected %g\n", forward, dkt_new, kt_new, kt_new_expected);
    fprintf(stderr, "  backward %g, dkt_old %g, kt_old %g, expected %g\n", -backward, dkt_old, kt_old, kt_old_expected);
    fprintf(stderr, "  dkt_new_deterministic %g\n", dkt_new_deterministic);
    fprintf(stderr, "  ln(g_ratio): %g\n", lng_ratio);
    if (langevin_move__debug__ >= 3) {
      getchar();
    }
  }

  return lng_ratio;
}



/* return if the new temperature proposed by the Langevin equation
 * should be accepted according to the Metropolisation rule */
static int langevin_move__accept(
    const langevin_move_proposal_t *proposal,
    langevin_t *langevin,
    mb_t *mb,
    mtrng_t *rng)
{
  int ib_new;
  double neg_dlnwf_dbeta_new;
  double invwf_old = proposal->invwf;
  double invwf_new;
  double kt_new_expected;
  double lnp_ratio;
  double lng_ratio;
  double ln_fac;

  if (proposal->beta_new > mb->bmax || proposal->beta_new < mb->bmin) {
    return 0;
  }

  ib_new = mb__beta_to_index(mb, proposal->beta_new, 0);
  invwf_new = mb_betadist__calc_inv_weight(mb->betadist,
      proposal->beta_new, &neg_dlnwf_dbeta_new, NULL, NULL);

  // ln [ p(kT_new) / p(kT_old) ]
  lnp_ratio = langevin_move__calc_lnp_ratio(
      langevin,
      mb,
      proposal->beta_old, proposal->beta_new,
      proposal->current_energy,
      invwf_old, invwf_new);

  kt_new_expected = proposal->kt_old + proposal->dkt_deterministic;

  // ln [ g(kT_new -> kT_old) / g(kT_old -> kT_new) ]
  lng_ratio = langevin_move__calc_lng_ratio(
      langevin,
      mb,
      proposal->kt_old,
      proposal->kt_new,
      proposal->current_energy,
      kt_new_expected,
      ib_new,
      neg_dlnwf_dbeta_new,
      proposal->time_step);

  //printf("beta %g => %g, lnp %g, lng %g, sum %g\n", beta_old, beta_new, lnp_ratio, lng_ratio, lnp_ratio + lng_ratio);
  //getchar();

  // ln [ prob(kT' -> kT) / prob(kT -> kT') ]
  ln_fac = lnp_ratio + lng_ratio;

  // min{1, exp(ln_fac)}
  if (ln_fac > 0) {

    return 1;

  } else {

    double r = mtrng_rand01(rng);

    if (r < exp(ln_fac)) {
      return 1;
    } else {
      return 0;
    }

  }

}



/* Langevin integration with Metropolis correction */
double langevin__move_corrected(
    langevin_t *langevin,
    mb_t *mb,
    double current_energy,
    double beta_old,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    mtrng_t *rng,
    double *bin_av_energy)
{
  langevin_move_proposal_t proposal[1];

  int stride_moderated;
  int accepted;

  if (!langevin_move__check_min_visits(langevin, mb, beta_old)) {
    return beta_old;
  }

  //fprintf(stderr, "langevin_move__debug__ %d\n", langevin_move__debug__); getchar();

  langevin_move__propose(
      proposal,
      langevin, mb,
      current_energy,
      beta_old, ib,
      invwf, neg_dlnwf_dbeta,
      use_cheap_av_energy_for_forward_move,
      apply_dkt_max,
      rng,
      bin_av_energy);

  stride_moderated = langevin_move__moderate_stride(
      proposal,
      langevin,
      mb);

  // compute the acceptance ratio
  accepted = langevin_move__accept(
      proposal,
      langevin,
      mb,
      rng);

  if (accepted) {
    mb->beta = proposal->beta_new;
  } else {
    langevin->rejects += 1.0;
  }

  langevin->total += 1.0;

  if (langevin_move__debug__) {
    fprintf(stderr, "langevin__move_corrected():\n");
    fprintf(stderr, "  beta %g => %g\n", proposal->beta_old, proposal->beta_new);
    fprintf(stderr, "  kT %g => %g\n", proposal->kt_old, proposal->kt_new);
    fprintf(stderr, "  stride_moderated %d\n", stride_moderated);
    fprintf(stderr, "  accepted %d\n", accepted);
    fprintf(stderr, "\n");
    if (langevin_move__debug__ >= 3) {
      getchar();
    }
  }

  return mb->beta;
}



double langevin__move(
    langevin_t *langevin,
    mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    mtrng_t *rng,
    double *bin_av_energy)
{

  if (langevin->corrected) {

    return langevin__move_corrected(
        langevin, mb,
        current_energy, beta, ib,
        invwf, neg_dlnwf_dbeta,
        rng, bin_av_energy);

  } else {

    return langevin__move_simple(
        langevin, mb,
        current_energy, beta, ib,
        invwf, neg_dlnwf_dbeta,
        rng, bin_av_energy);

  }

}



#endif
