/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT__MB_UTILS_H__
#define AT__MB_UTILS_H__

#include "mb__def.h"



/* write various averages to ze_file */
int mb__write_ze_file(mb_t *mb, const char *fname);

int mb__beta_to_index(mb_t *mb, double bet, int check);

/* add energy and bet */
void mb__add(mb_t *mb, double e, double bet,
      int *pib, double *pinvwf, double *ndlnwfdbeta);

/* recompute all average energy */
void mb__refresh_et(mb_t *mb, int reps);

#endif
