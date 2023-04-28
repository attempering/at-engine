/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT_MB_UTILS_H__
#define AT_MB_UTILS_H__

#include "at_mb__def.h"



/* write various averages to ze_file */
int at_mb__write_ze_file(at_mb_t *mb, const char *fname);

int at_mb__beta_to_index(at_mb_t *mb, double bet, int check);

/* add energy and bet */
void at_mb__add(at_mb_t *mb, double e, double bet,
      int *pib, double *pinvwf, double *ndlnwfdbeta);

/* recompute all average energy */
void at_mb__refresh_et(at_mb_t *mb, int reps);

#endif
