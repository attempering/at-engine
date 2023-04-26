#ifndef AT__MB_IIE_ET_CALC_H__
#define AT__MB_IIE_ET_CALC_H__

/* integral identity based estimator of the bin-averaged energy
 * along with cache
 */
#include "mb_iie_et__def.h"


typedef struct mb_t_ mb_t;
typedef struct sm_t_ sm_t;

/* calculate the estimated average energy Et of bin ib
 * using the integral identity estimator.
 *
 * Note that the calculated value is a bin-averaged one,
 * not one at either boundary of the bin.
 * */
double mb_iie_et__calc_et(mb_iie_t *iie, int ib);


#endif
