#ifndef AT__MB_IIE_LR_CALC_H__
#define AT__MB_IIE_LR_CALC_H__



#include "mb_iie_lr__def.h"



/* initialization for a particular estimate
 * this includes setting up proper accumulators */
void mb_iie_lr__init_instance(mb_iie_lr_t *lr,
    int type, int win_div,
    int ib, int js, int jt);

/* collect moments */
void mb_iie_lr__collect_moments(mb_iie_lr_t *lr);

/* to compute the coefficients of linear combination
 * for the left-right IIE */
double mb_iie_lr__balance_moments(mb_iie_lr_t *lr);



#endif
