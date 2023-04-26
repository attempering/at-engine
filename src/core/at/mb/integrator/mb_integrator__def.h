#ifndef AT__MB_INTEGRATOR__DEF_H__
#define AT__MB_INTEGRATOR__DEF_H__

typedef struct at_mb_zerofiller_t_ at_mb_zerofiller_t;

/* parameters for the integrator */
typedef struct at_mb_integrator_t_ {
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

  /* reference to the zero-filler */
  at_mb_zerofiller_t *zerofiller;

} at_mb_integrator_t;

#endif
