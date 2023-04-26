#ifndef AT__MB_IIE_ET_BASIC_H__
#define AT__MB_IIE_ET_BASIC_H__

#include "mb_iie_et__def.h"

typedef struct at_mb_t_ at_mb_t;
typedef struct zcom_cfg_t_ zcom_cfg_t;

void at_mb_iie_et__cfg_init(at_mb_iie_et_t *et, at_mb_t *mb, zcom_cfg_t *cfg, int silent);

void at_mb_iie_et__finish(at_mb_iie_et_t *et);

void at_mb_iie_et__clear(at_mb_iie_et_t *et);

#endif
