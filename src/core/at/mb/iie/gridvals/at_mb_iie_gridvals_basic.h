#ifndef AT_MB_IIE_GRIDVALS_BASIC_H__
#define AT_MB_IIE_GRIDVALS_BASIC_H__


#include "at_mb_iie_gridvals__def.h"

typedef struct at_mb_t_ at_mb_t;
typedef struct zcom_cfg_t_ zcom_cfg_t;


void at_mb_iie_gridvals__cfg_init(at_mb_iie_gridvals_t *gridvals, at_mb_t *mb,
    zcom_cfg_t *cfg, int silent);

void at_mb_iie_gridvals__clear(at_mb_iie_gridvals_t *gridvals);

void at_mb_iie_gridvals__finish(at_mb_iie_gridvals_t *gridvals);


#endif
