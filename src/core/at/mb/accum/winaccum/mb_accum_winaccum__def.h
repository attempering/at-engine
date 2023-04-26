#ifndef AT__MB_ACCUM_WINACCUM__DEF_H__
#define AT__MB_ACCUM_WINACCUM__DEF_H__


#include "../../sm/mb_sm.h"
#include "../../win/mb_win__def.h"


typedef struct at_mb_accum_winaccum_item_t_ {
  at_mb_sm_t *sums;
  int i;
  int js, jt;
  double amplifier;
  double visits;
} at_mb_accum_winaccum_item_t;

typedef struct at_mb_accum_winaccum_t_ {
  int n;
  unsigned flags;
  at_mb_win_t *win; // reference handler
  at_mb_accum_winaccum_item_t *items;
} at_mb_accum_winaccum_t;


#define MB_ACCUM_MIN_SIZE 1e-6


#endif

