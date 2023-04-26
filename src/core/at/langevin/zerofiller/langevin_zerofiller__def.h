#ifndef AT__LANGEVIN_ZEROFILLER__DEF_H__
#define AT__LANGEVIN_ZEROFILLER__DEF_H__

typedef struct at_mb_t_ at_mb_t;

/* parameters for the zerofiller */
typedef struct at_langevin_zerofiller_t_ {
  int n; /* same as mb->n */
  double    beta_min; /* same as mb->bmin */
  double    beta_max; /* same as mb->bmax */
  double    beta_del; /* same as mb->bdel */

  double   *w;        /* weights */
  double   *vals;     /* filled values */
  int      *has_vals;

  int      *ib_from_left;
  int      *ib_from_right;

  at_mb_t *mb;
} at_langevin_zerofiller_t;

typedef double (*bin_value_get_func_t)(at_mb_t *mb, int ib, int *has_val, double *w);


#endif
