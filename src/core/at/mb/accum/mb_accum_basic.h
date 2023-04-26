#ifndef AT__MB_ACCUM_BASIC_H__
#define AT__MB_ACCUM_BASIC_H__


#include "mb_accum__def.h"
#include "../mb__def.h"
#include "../win/mb_win.h"
#include "../shk/mb_shk.h"
#include <stdio.h>


sm_t *mb_accum__get_sums(mb_accum_t *accum, int j);

sm_t *mb_accum__get_proper_sums(mb_accum_t *accum, int win_id, int ib);

double mb_accum__get_window_visits(mb_accum_t *accum, int win_id);

void mb_accum__add(mb_accum_t *accum, int j, double invwf, double e, int do_2nd,
    mb_shk_t *shk, double total_visits);

void mb_accum__calc_win_total(mb_accum_t *accum);

void mb_accum__init(mb_accum_t *accum, int n, mb_win_t *win, unsigned flags);

void mb_accum__clear(mb_accum_t *accum);

void mb_accum__finish(mb_accum_t *accum);

#endif
