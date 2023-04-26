#ifndef AT__MB_INTEGRATOR_H__
#define AT__MB_INTEGRATOR_H__



/* integrator of the estimated averager over a temperature range */



#include "../mb__def.h"


int mb_integrator__init(mb_integrator_t *intgr, mb_t *mb);

void mb_integrator__finish(mb_integrator_t *intgr);

double mb_integrator__integrate(mb_integrator_t *intgr, double beta_from, double beta_to);


#endif
