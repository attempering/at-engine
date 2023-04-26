#ifndef AT__MB_IIE_ET_CALC_H__
#define AT__MB_IIE_ET_CALC_H__

/* integral identity based estimator of the bin-averaged energy
 * along with cache
 */
#include "mb_iie_et__def.h"


typedef struct at_mb_t_ at_mb_t;
typedef struct at_mb_sm_t_ at_mb_sm_t;

/* calculate the estimated average energy Et of bin ib
 * using the integral identity estimator.
 *
 * Note that the calculated value is a bin-averaged one,
 * not one at either boundary of the bin.
 * */
double at_mb_iie_et__calc_et(at_mb_iie_t *iie, int ib);


#endif
