#ifndef AT_LANGEVIN_INTEGRATOR__DEF_H__
#define AT_LANGEVIN_INTEGRATOR__DEF_H__

typedef struct at_langevin_zerofiller_t_ at_langevin_zerofiller_t;

#include "../../mb/accum/at_mb_accum__def.h"

/* parameters for the integrator */
typedef struct at_langevin_integrator_t_ {
  int n; /* same as mb->n */
  double    beta_min; /* same as mb->bmin */
  double    beta_max; /* same as mb->bmax */
  double    beta_del; /* same as mb->bdel */

  int       sgn;
  double    beta_begin;
  double    beta_end;
  int       ib_begin;
  int       ib_end;
  double    dbeta_left;
  double    dbeta_right;

  double    *vals; /* values without zerofiller */

  int use_zerofiller;

  /* reference to the zero-filler */
  at_langevin_zerofiller_t zerofiller[1];

  at_mb_accum_t *accum; /* reference to the accumulator */

} at_langevin_integrator_t;

#endif
