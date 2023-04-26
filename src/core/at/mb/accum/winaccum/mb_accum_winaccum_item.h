#ifndef AT__MB_ACCUM_WINACCUM_ITEM_H__
#define AT__MB_ACCUM_WINACCUM_ITEM_H__


#include "mb_accum_winaccum__def.h"
#include "../../mb__def.h"
#include "../../win/mb_win.h"
#include "../../shk/mb_shk.h"
#include <stdio.h>



sm_t *mb_accum_winaccum_item__get_sums(mb_accum_winaccum_item_t *item, int j);

void mb_accum_winaccum_item__normalize(mb_accum_winaccum_item_t *item);

double mb_accum_winaccum_item__calc_total(mb_accum_winaccum_item_t *item);

void mb_accum_winaccum_item__add(
    mb_accum_winaccum_item_t *item,
    int ib,
    double invwf, double e, int do_2nd,
    mb_shk_t *shk,
    double total_visits);



void mb_accum_winaccum_item__init(mb_accum_winaccum_item_t *item,
    int i, mb_win_t *win);

void mb_accum_winaccum_item__clear(mb_accum_winaccum_item_t *item);

void mb_accum_winaccum_item__finish(mb_accum_winaccum_item_t *item);

int mb_accum_winaccum_item__read_binary(mb_accum_winaccum_item_t *item, FILE *fp, int endn);

int mb_accum_winaccum_item__write_binary(mb_accum_winaccum_item_t *item, FILE *fp);



#endif
