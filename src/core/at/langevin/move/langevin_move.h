#ifndef AT__LANGEVIN_MOVE_H__
#define AT__LANGEVIN_MOVE_H__



#include "../../mb/mb__def.h"
#include "../langevin__def.h"
#include "../../zcom/rng/rng.h"



/* estimate `bin_av_energy` at the current temperature `bet`
 * calculate the new beta after integrating
 * the Langevin equation based on kT = 1/beta;
 * neg_dlnwf_dbeta = -d[ln(w*f)]/d(beta) */
double langevin__move(
    langevin_t *langevin,
    mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    mtrng_t *rng,
    double *bin_av_energy);

/* Langevin equation without Metropolis correction */
double langevin__move_simple(
    langevin_t *langevin,
    mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    mtrng_t *rng,
    double *bin_av_energy);

/* Langevin equation with Metropolisation correction */
double langevin__move_corrected(
    langevin_t *langevin,
    mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    mtrng_t *rng,
    double *bin_av_energy);

/* return the acceptance ratio so far (for the corrected move) */
double langevin_move__get_acceptance_ratio(langevin_t *langevin);


int langevin_move__debug__;

#endif
