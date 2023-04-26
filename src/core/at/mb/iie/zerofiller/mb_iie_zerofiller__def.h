#ifndef AT__MB_IIE_ZEROFILLER__DEF_H__
#define AT__MB_IIE_ZEROFILLER__DEF_H__

typedef struct mb_t_ mb_t;

typedef struct mb_iie_zerofiller_item_t_ {
  double w;
  double mean;
  double var;
} mb_iie_zerofiller_item_t;

typedef struct mb_iie_zerofiller_t_ {
  int n; /* same as mb->n */
  double    beta_min; /* same as mb->bmin */
  double    beta_max; /* same as mb->bmax */
  double    beta_del; /* same as mb->bdel */

  /* In an earlier design, we kept a record of *raw_vals
   * in addition to the *vals. This is now abandoned.
   * After the simulation accumulates enough data points,
   * all data points are filled. In such a case,
   * the zerofiller should perform as few operations
   * as possible to achieve the best performance.
   * Copying values from raw_vals to vals would be
   * too time-consuming in a case. */
  mb_iie_zerofiller_item_t   *vals;     /* filled values */
  int      *has_vals;

  int      *ib_from_left;
  int      *ib_from_right;

  mb_t *mb;
} mb_iie_zerofiller_t;


#endif