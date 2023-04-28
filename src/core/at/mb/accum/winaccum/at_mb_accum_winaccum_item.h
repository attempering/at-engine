#ifndef AT_MB_ACCUM_WINACCUM_ITEM_H__
#define AT_MB_ACCUM_WINACCUM_ITEM_H__


#include "at_mb_accum_winaccum__def.h"
#include "../../at_mb__def.h"
#include "../../win/at_mb_win.h"
#include "../../shk/at_mb_shk.h"
#include <stdio.h>



at_mb_sm_t *at_mb_accum_winaccum_item__get_sums(at_mb_accum_winaccum_item_t *item, int j);

void at_mb_accum_winaccum_item__normalize(at_mb_accum_winaccum_item_t *item);

double at_mb_accum_winaccum_item__calc_total(at_mb_accum_winaccum_item_t *item);

void at_mb_accum_winaccum_item__add(
    at_mb_accum_winaccum_item_t *item,
    int ib,
    double invwf, double e, int do_2nd,
    at_mb_shk_t *shk,
    double total_visits);



void at_mb_accum_winaccum_item__init(at_mb_accum_winaccum_item_t *item,
    int i, at_mb_win_t *win);

void at_mb_accum_winaccum_item__clear(at_mb_accum_winaccum_item_t *item);

void at_mb_accum_winaccum_item__finish(at_mb_accum_winaccum_item_t *item);

int at_mb_accum_winaccum_item__read_binary(at_mb_accum_winaccum_item_t *item, FILE *fp, int endn);

int at_mb_accum_winaccum_item__write_binary(at_mb_accum_winaccum_item_t *item, FILE *fp);



#endif
