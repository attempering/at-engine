#ifndef AT__MB_ACCUM_WINACCUM_BASIC_H__
#define AT__MB_ACCUM_WINACCUM_BASIC_H__


#include "mb_accum_winaccum__def.h"
#include "../../mb__def.h"
#include "../../win/mb_win.h"
#include "../../shk/mb_shk.h"
#include <stdio.h>


void mb_accum_winaccum__calc_win_total(mb_accum_winaccum_t *winaccum, double *win_total);

void mb_accum_winaccum__init(mb_accum_winaccum_t *winaccum, int n, mb_win_t *win, unsigned flags);

void mb_accum_winaccum__clear(mb_accum_winaccum_t *winaccum);

void mb_accum_winaccum__finish(mb_accum_winaccum_t *winaccum);

void mb_accum_winaccum__add(
    mb_accum_winaccum_t *winaccum,
    int j,
    double invwf, double e, int do_2nd,
    mb_shk_t *shk, double total_visits);

#endif