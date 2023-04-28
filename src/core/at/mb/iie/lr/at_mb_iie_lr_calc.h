#ifndef AT_MB_IIE_LR_CALC_H__
#define AT_MB_IIE_LR_CALC_H__



#include "at_mb_iie_lr__def.h"



/* initialization for a particular estimate
 * this includes setting up proper accumulators */
void at_mb_iie_lr__init_instance(at_mb_iie_lr_t *lr,
    int type, int win_div,
    int ib, int js, int jt);

/* collect moments */
void at_mb_iie_lr__collect_moments(at_mb_iie_lr_t *lr);

/* to compute the coefficients of linear combination
 * for the left-right IIE */
double at_mb_iie_lr__balance_moments(at_mb_iie_lr_t *lr);



#endif
