#ifndef AT__MB_INTEGRATOR_H__
#define AT__MB_INTEGRATOR_H__



/* integrator of the estimated averager over a temperature range */



#include "../mb__def.h"


int at_mb_integrator__init(at_mb_integrator_t *intgr, at_mb_t *mb);

void at_mb_integrator__finish(at_mb_integrator_t *intgr);

double at_mb_integrator__integrate(at_mb_integrator_t *intgr, double beta_from, double beta_to);


#endif
