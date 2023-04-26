#ifndef AT__SM_H__
#define AT__SM_H__


/* simple accumulator */


#include <stdio.h>


typedef struct sm_t_ {
  double s;
  double se;
  double se2;
  double se3;
} sm_t;

/* multiply everything in sm by a factor `f` */
void sm__scale(sm_t *sm, double f);

/* add a new data point of weight `w` to the accumulator */
void sm__add(sm_t *sm, double w, double e, int do_third_order);

double sm__get_mean(sm_t *sm, double min_size);

double sm__get_var(sm_t *sm, double min_size);

double sm__get_std(sm_t *sm, double min_size);

int sm__init(sm_t* sm);

void sm__clear(sm_t *sm);

int sm__read_binary(sm_t *sm, FILE *fp, int endn);

int sm__write_binary(sm_t *sm, FILE *fp);

void sm__manifest(sm_t *sm, FILE *fp, int arrmax);

#endif
