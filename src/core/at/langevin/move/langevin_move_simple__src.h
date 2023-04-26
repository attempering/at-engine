#ifndef AT__LANGEVIN_MOVE_SIMPLE__SRC_H__
#define AT__LANGEVIN_MOVE_SIMPLE__SRC_H__



#include "langevin_move_basic.h"



/* Langevin integration */
double langevin__move_simple(
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
  const int cheap_av_energy = 0;
  const int apply_dkt_max = 1;

  langevin_move_proposal_t proposal[1];

  //fprintf(stderr, "1. beta %g, ib %d\n", beta_old, ib);

  if (!langevin_move__check_min_visits(langevin, mb, beta_old)) {
    return beta_old;
  }

  //fprintf(stderr, "2. beta %g, ib %d\n", beta_old, ib);

  langevin_move__propose(
      proposal,
      langevin, mb,
      current_energy,
      beta_old, ib,
      invwf, neg_dlnwf_dbeta,
      cheap_av_energy,
      apply_dkt_max,
      rng, bin_av_energy);

  //fprintf(stderr, "3. beta %g => %g, %g\n", beta_old, proposal->beta_new, *bin_av_energy);

  langevin_move__moderate_stride(
      proposal,
      langevin,
      mb);

  if (proposal->beta_new < mb->bmax
   && proposal->beta_new > mb->bmin) {
    mb->beta = proposal->beta_new;
  } else {
    mb->beta = proposal->beta_old;
    langevin->rejects += 1.0;
  }

  langevin->total += 1.0;

  return mb->beta;
}



#endif
