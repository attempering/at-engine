#ifndef AT__MB_IIE_LR_BASIC_H__
#define AT__MB_IIE_LR_BASIC_H__


#include "mb_iie_lr__def.h"


typedef struct zcom_cfg_t_ zcom_cfg_t;

/* global initialization */
void at_mb_iie_lr__cfg_init(at_mb_iie_lr_t *lr, at_mb_iie_zerofiller_t *zf,
    at_mb_t *mb, zcom_cfg_t *cfg, int silent);

void at_mb_iie_lr__finish(at_mb_iie_lr_t *lr);

#endif

