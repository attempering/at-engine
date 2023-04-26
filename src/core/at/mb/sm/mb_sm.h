#ifndef AT__MB_SM_H__
#define AT__MB_SM_H__


#include "mb_sm__def.h"

/* multiply everything in sm by a factor `f` */
void at_mb_sm__scale(at_mb_sm_t *sm, double f);

/* add a new data point of weight `w` to the accumulator */
void at_mb_sm__add(at_mb_sm_t *sm, double w, double e, int do_third_order);

double at_mb_sm__get_mean(at_mb_sm_t *sm, double min_size);

double at_mb_sm__get_var(at_mb_sm_t *sm, double min_size);

double at_mb_sm__get_std(at_mb_sm_t *sm, double min_size);

int at_mb_sm__init(at_mb_sm_t* sm);

void at_mb_sm__clear(at_mb_sm_t *sm);

int at_mb_sm__read_binary(at_mb_sm_t *sm, FILE *fp, int endn);

int at_mb_sm__write_binary(at_mb_sm_t *sm, FILE *fp);

void at_mb_sm__manifest(at_mb_sm_t *sm, FILE *fp, int arrmax);

#endif
