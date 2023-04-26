#ifndef AT__MB_IIE_GRIDVALS_CALC_H__
#define AT__MB_IIE_GRIDVALS_CALC_H__

/* integral-identity-based estimators for thermodynamic quantities at the grid points */

#include "mb_iie_gridvals__def.h"

typedef struct at_mb_t_ at_mb_t;

/* calculate the estimated average energy Et, Cv and partition function
 * at all grid points */
void at_mb_iie_gridvals__calc(at_mb_t *mb);


#endif
