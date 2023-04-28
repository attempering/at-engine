#ifndef AT_MB_ACCUM_BASIC_H__
#define AT_MB_ACCUM_BASIC_H__


#include "at_mb_accum__def.h"
#include "../at_mb__def.h"
#include "../win/at_mb_win.h"
#include "../shk/at_mb_shk.h"
#include <stdio.h>


at_mb_sm_t *at_mb_accum__get_sums(at_mb_accum_t *accum, int j);

at_mb_sm_t *at_mb_accum__get_proper_sums(at_mb_accum_t *accum, int win_id, int ib);

double at_mb_accum__get_window_visits(at_mb_accum_t *accum, int win_id);

void at_mb_accum__add(at_mb_accum_t *accum, int j, double invwf, double e, int do_2nd,
    at_mb_shk_t *shk, double total_visits);

void at_mb_accum__calc_win_total(at_mb_accum_t *accum);

void at_mb_accum__init(at_mb_accum_t *accum, int n, at_mb_win_t *win, unsigned flags);

void at_mb_accum__clear(at_mb_accum_t *accum);

void at_mb_accum__finish(at_mb_accum_t *accum);

#endif
