#ifndef AT__LANGEVIN_MOVE_BASIC_H__
#define AT__LANGEVIN_MOVE_BASIC_H__




#include "../../mb/mb__def.h"
#include "../langevin__def.h"
#include "../../zcom/rng/rng.h"
#include "langevin_move_et.h"



typedef struct langevin_move_proposal_t_ {
  double current_energy;
  double invwf;
  double time_step;
  double beta_old;
  double kt_old;
  double dkt_deterministic;
  double dkt;
  double kt_new;
  double beta_new;
} langevin_move_proposal_t;


/* check if the minimum stay requirement has been met */
int at_langevin_move__check_min_visits(
    at_langevin_t *langevin,
    at_mb_t *mb,
    double beta_old);

int at_langevin_move__propose(
    langevin_move_proposal_t *proposal,
    at_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta_old,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    int cheap_av_energy,
    int apply_dkt_max,
    zcom_mtrng_t *rng,
    double *bin_av_energy);

/* moderate the stride in the beta space */
int at_langevin_move__moderate_stride(
    langevin_move_proposal_t *proposal,
    at_langevin_t *langevin,
    at_mb_t *mb);

#endif
