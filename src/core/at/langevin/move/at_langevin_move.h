#ifndef AT_LANGEVIN_MOVE_H__
#define AT_LANGEVIN_MOVE_H__



#include "../../mb/at_mb__def.h"
#include "../at_langevin__def.h"
#include "../../../zcom/zcom.h"



/* estimate `bin_av_energy` at the current temperature `bet`
 * calculate the new beta after integrating
 * the Langevin equation based on kT = 1/beta;
 * neg_dlnwf_dbeta = -d[ln(w*f)]/d(beta) */
double at_langevin__move(
    at_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    zcom_mtrng_t *rng,
    double *bin_av_energy);

/* Langevin equation without Metropolis correction */
double at_langevin__move_simple(
    at_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    zcom_mtrng_t *rng,
    double *bin_av_energy);

/* Langevin equation with Metropolisation correction */
double at_langevin__move_corrected(
    at_langevin_t *langevin,
    at_mb_t *mb,
    double current_energy,
    double beta,
    int ib,
    double invwf,
    double neg_dlnwf_dbeta,
    zcom_mtrng_t *rng,
    double *bin_av_energy);

/* return the acceptance ratio so far (for the corrected move) */
double at_langevin_move__get_acceptance_ratio(at_langevin_t *langevin);


int at_langevin_move__debug__;

#endif
