#ifndef AT__LANGEVIN_MOVE_ET__SRC_H__
#define AT__LANGEVIN_MOVE_ET__SRC_H__



#include "langevin_move_et.h"

/* implementation headers */
#include "../../mb/mb.h"



static double langevin_move__calc_et_single_bin(
    langevin_t *langevin,
    mb_t *mb,
    int ib,
    double def_val)
{
  double et;
  sm_t *sm = mb_accum__get_proper_sums(mb->accum, ib, ib);

  if (sm->s > MB_ACCUM_MIN_SIZE) {
    et = sm__get_mean(sm, MB_ACCUM_MIN_SIZE);
  } else {
    et = def_val;
  }

  return et;
}



static double langevin_move__calc_et_iie(
    langevin_t *langevin,
    mb_t *mb,
    int ib,
    double def_val)
{
  mb_iie_t *iie = mb->iie;
  mb_iie_lr_t *lr = iie->lr;
  double et = mb_iie_et__calc_et_cached(iie, ib);

  if (lr->success && lr->quality) {
    return et;
  } else {
    return def_val;
  }

}



double langevin_move__calc_et(
    langevin_t *langevin,
    mb_t *mb,
    int ib,
    double def_val,
    int cheap_av_energy)
{
  if (cheap_av_energy) {

    return langevin_move__calc_et_single_bin(
        langevin,
        mb,
        ib,
        def_val);

  } else {

    return langevin_move__calc_et_iie(
        langevin,
        mb,
        ib,
        def_val);

  }

}



/* deterministic part of the kT-based Langevin equation */
double langevin_move__calc_dkt_deterministic(
    langevin_t *langevin,
    mb_t *mb, 
    int ib,
    double time_step,
    double neg_dlnwf_dbeta,
    double current_energy,
    double *bin_av_energy,
    int cheap_av_energy)
{
  double delta;

  *bin_av_energy = langevin_move__calc_et(
      langevin,
      mb,
      ib,
      current_energy,
      cheap_av_energy);

  delta = (current_energy - *bin_av_energy + neg_dlnwf_dbeta);

  if (langevin_move__debug__ >= 2) {
    fprintf(stderr, "langevin_move__calc_dkt_deterministic()\n");
    fprintf(stderr, "  cheap_av_energy %d\n", cheap_av_energy);
    fprintf(stderr, "  ib %d\n", (int) ib);
    fprintf(stderr, "  current_energy %g\n", current_energy);
    fprintf(stderr, "  bin_av_energy %g\n", *bin_av_energy);
    fprintf(stderr, "  neg_dlnwf_dbeta %g\n", neg_dlnwf_dbeta);
    fprintf(stderr, "  delta %g\n", delta);
    fprintf(stderr, "  dkt %g\n", delta * time_step);
  }

  return delta * time_step;
}



#endif
