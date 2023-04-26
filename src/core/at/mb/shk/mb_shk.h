#ifndef AT__MB_SHK_H__
#define AT__MB_SHK_H__

/* shrinking rate of the adaptive averaging scheme */

#include <stdio.h>

#include "mb_shk__def.h"


typedef struct mb_t_ mb_t;
typedef struct cfg_t_ cfg_t;

int mb_shk__cfg_init(mb_shk_t *shk, cfg_t *cfg, mb_t *mb, int m, int silent);

void mb_shk__finish(mb_shk_t *shk);

void mb_shk__manifest(const mb_shk_t *shk, FILE *fp, int arrmax);

double mb_shk__calc_shk_base(mb_shk_t *shk, double total_visits);

double mb_shk__calc_inverse_gamma(mb_shk_t *shk, double total_visits, int ib);

#endif
