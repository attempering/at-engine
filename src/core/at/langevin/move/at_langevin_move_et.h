#ifndef AT_LANGEVIN_MOVE_ET_H__
#define AT_LANGEVIN_MOVE_ET_H__




#include "../../mb/at_mb__def.h"
#include "../at_langevin__def.h"



double at_langevin_move__calc_et(
    at_langevin_t *langevin,
    at_mb_t *mb,
    int ib,
    double def_val,
    int check_av_energy);


/* deterministic part of the kT-based Langevin equation */
double at_langevin_move__calc_dkt_deterministic(
    at_langevin_t *langevin,
    at_mb_t *mb, 
    int ib,
    double time_step,
    double neg_dlnwf_dbeta,
    double current_energy,
    double *bin_av_energy,
    int cheap_av_energy);


#endif
