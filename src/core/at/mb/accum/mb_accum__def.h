#ifndef AT__MB_ACCUM__DEF_H__
#define AT__MB_ACCUM__DEF_H__


#include "../../sm/sm.h"
#include "../win/mb_win.h"


#include "winaccum/mb_accum_winaccum__def.h"


typedef struct mb_accum_t_ {
  int n; /* number of bins == mb->n */

  /* regular accumulators */
  sm_t *sums;

  /* for multiple bin averager */
  int m;
  int use_winaccum;

  double *win_total; /* total of sum.s over a multiple-bin temperature window */

  mb_accum_winaccum_t winaccum[1];

  mb_win_t *win; /* reference to mb->win */

  unsigned flags;

} mb_accum_t;


#define MB_ACCUM_MIN_SIZE 1e-6


#endif

