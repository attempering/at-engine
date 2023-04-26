#ifndef AT__MB_IIE_GRIDVALS_BASIC_H__
#define AT__MB_IIE_GRIDVALS_BASIC_H__


#include "mb_iie_gridvals__def.h"

typedef struct mb_t_ mb_t;
typedef struct cfg_t_ cfg_t;


void mb_iie_gridvals__cfg_init(mb_iie_gridvals_t *gridvals, mb_t *mb,
    cfg_t *cfg, int silent);

void mb_iie_gridvals__clear(mb_iie_gridvals_t *gridvals);

void mb_iie_gridvals__finish(mb_iie_gridvals_t *gridvals);


#endif
