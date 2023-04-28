#ifndef AT_LANGEVIN_MOVE_ET__SRC_H__
#define AT_LANGEVIN_MOVE_ET__SRC_H__



#include "at_langevin_move_et.h"

/* implementation headers */
#include "../../mb/at_mb.h"



static double at_langevin_move__calc_et_single_bin(
    at_langevin_t *langevin,
    at_mb_t *mb,
    int ib,
    double def_val)
{
  double et;
  at_mb_sm_t *sm = at_mb_accum__get_proper_sums(mb->accum, ib, ib);

  if (sm->s > AT_MB_ACCUM_MIN_SIZE) {
    et = at_mb_sm__get_mean(sm, AT_MB_ACCUM_MIN_SIZE);
  } else {
    et = def_val;
  }

  return et;
}



static double at_langevin_move__calc_et_iie(
    at_langevin_t *langevin,
    at_mb_t *mb,
    int ib,
    double def_val)
{
  at_mb_iie_t *iie = mb->iie;
  at_mb_iie_lr_t *lr = iie->lr;
  double et = at_mb_iie_et__calc_et_cached(iie, ib);

  if (at_langevin_move__debug__ >= 2) {
    fprintf(stderr, "at_langevin_move__calc_et_iie() %s:%d\n", __FILE__, __LINE__);
    fprintf(stderr, "  et %g (default %g)\n", et, def_val);
    fprintf(stderr, "  ib %d\n", (int) ib);
    fprintf(stderr, "  success %d, quality %d\n", lr->success, lr->quality);
    fprintf(stderr, "  cache enabled %d, hit %d\n",
        iie->et->cache_params->enabled, iie->et->cache_hit);
    fprintf(stderr, "\n");
  }

  if (lr->success && lr->quality) {
    return et;
  } else {
    return def_val;
  }

}



double at_langevin_move__calc_et(
    at_langevin_t *langevin,
    at_mb_t *mb,
    int ib,
    double def_val,
    int cheap_av_energy)
{
  if (cheap_av_energy) {

    return at_langevin_move__calc_et_single_bin(
        langevin,
        mb,
        ib,
        def_val);

  } else {

    return at_langevin_move__calc_et_iie(
        langevin,
        mb,
        ib,
        def_val);

  }

}



/* deterministic part of the kT-based Langevin equation */
double at_langevin_move__calc_dkt_deterministic(
    at_langevin_t *langevin,
    at_mb_t *mb, 
    int ib,
    double time_step,
    double neg_dlnwf_dbeta,
    double current_energy,
    double *bin_av_energy,
    int cheap_av_energy)
{
  double delta;

  *bin_av_energy = at_langevin_move__calc_et(
      langevin,
      mb,
      ib,
      current_energy,
      cheap_av_energy);

  delta = (current_energy - *bin_av_energy + neg_dlnwf_dbeta);

  if (at_langevin_move__debug__ >= 2) {
    fprintf(stderr, "at_langevin_move__calc_dkt_deterministic() %s:%d\n", __FILE__, __LINE__);
    fprintf(stderr, "  cheap_av_energy %d\n", cheap_av_energy);
    fprintf(stderr, "  ib %d\n", (int) ib);
    fprintf(stderr, "  current_energy %g\n", current_energy);
    fprintf(stderr, "  bin_av_energy %g\n", *bin_av_energy);
    fprintf(stderr, "  neg_dlnwf_dbeta %g\n", neg_dlnwf_dbeta);
    fprintf(stderr, "  delta %g\n", delta);
    fprintf(stderr, "  dkt %g\n", delta * time_step);
    fprintf(stderr, "\n");
  }

  return delta * time_step;
}



#endif
