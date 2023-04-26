#ifndef AT__MB_ACCUM_WINACCUM_BASIC_H__
#define AT__MB_ACCUM_WINACCUM_BASIC_H__


#include "mb_accum_winaccum__def.h"
#include "../../mb__def.h"
#include "../../win/mb_win.h"
#include "../../shk/mb_shk.h"
#include <stdio.h>


void at_mb_accum_winaccum__calc_win_total(at_mb_accum_winaccum_t *winaccum, double *win_total);

void at_mb_accum_winaccum__init(at_mb_accum_winaccum_t *winaccum, int n, at_mb_win_t *win, unsigned flags);

void at_mb_accum_winaccum__clear(at_mb_accum_winaccum_t *winaccum);

void at_mb_accum_winaccum__finish(at_mb_accum_winaccum_t *winaccum);

void at_mb_accum_winaccum__add(
    at_mb_accum_winaccum_t *winaccum,
    int j,
    double invwf, double e, int do_2nd,
    at_mb_shk_t *shk, double total_visits);

#endif