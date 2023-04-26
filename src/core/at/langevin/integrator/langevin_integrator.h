#ifndef AT__LANGEVIN_INTEGRATOR_H__
#define AT__LANGEVIN_INTEGRATOR_H__



/* integrator of the estimated averager over a temperature range */



#include "../../mb/mb__def.h"


int at_langevin_integrator__init(at_langevin_integrator_t *intgr,
    at_mb_t *mb, int use_zerofiller);

void at_langevin_integrator__finish(at_langevin_integrator_t *intgr);

double at_langevin_integrator__integrate(at_langevin_integrator_t *intgr,
    double beta_from, double beta_to);


#endif
