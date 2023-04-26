#ifndef AT__LANGEVIN_MOVE_ET_H__
#define AT__LANGEVIN_MOVE_ET_H__




#include "../../mb/mb__def.h"
#include "../langevin__def.h"



double langevin_move__calc_et(
    langevin_t *langevin,
    mb_t *mb,
    int ib,
    double def_val,
    int check_av_energy);


/* deterministic part of the kT-based Langevin equation */
double langevin_move__calc_dkt_deterministic(
    langevin_t *langevin,
    mb_t *mb, 
    int ib,
    double time_step,
    double neg_dlnwf_dbeta,
    double current_energy,
    double *bin_av_energy,
    int cheap_av_energy);


#endif
