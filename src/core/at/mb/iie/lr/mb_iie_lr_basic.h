#ifndef AT__MB_IIE_LR_BASIC_H__
#define AT__MB_IIE_LR_BASIC_H__


#include "mb_iie_lr__def.h"


typedef struct cfg_t_ cfg_t;

/* global initialization */
void mb_iie_lr__cfg_init(mb_iie_lr_t *lr, mb_iie_zerofiller_t *zf,
    mb_t *mb, cfg_t *cfg, int silent);

void mb_iie_lr__finish(mb_iie_lr_t *lr);

#endif

