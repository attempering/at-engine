#ifndef AT__MB_IIE_GRIDVALS_CALC_H__
#define AT__MB_IIE_GRIDVALS_CALC_H__

/* integral-identity-based estimators for thermodynamic quantities at the grid points */

#include "mb_iie_gridvals__def.h"

typedef struct mb_t_ mb_t;

/* calculate the estimated average energy Et, Cv and partition function
 * at all grid points */
void mb_iie_gridvals__calc(mb_t *mb);


#endif
