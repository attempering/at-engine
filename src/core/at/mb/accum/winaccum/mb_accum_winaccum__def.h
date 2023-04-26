#ifndef AT__MB_ACCUM_WINACCUM__DEF_H__
#define AT__MB_ACCUM_WINACCUM__DEF_H__


#include "../../../sm/sm.h"
#include "../../win/mb_win__def.h"


typedef struct mb_accum_winaccum_item_t_ {
  sm_t *sums;
  int i;
  int js, jt;
  double amplifier;
  double visits;
} mb_accum_winaccum_item_t;

typedef struct mb_accum_winaccum_t_ {
  int n;
  unsigned flags;
  mb_win_t *win; // reference handler
  mb_accum_winaccum_item_t *items;
} mb_accum_winaccum_t;


#define MB_ACCUM_MIN_SIZE 1e-6


#endif

