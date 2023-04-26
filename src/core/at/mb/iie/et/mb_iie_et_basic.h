#ifndef AT__MB_IIE_ET_BASIC_H__
#define AT__MB_IIE_ET_BASIC_H__

#include "mb_iie_et__def.h"

typedef struct mb_t_ mb_t;
typedef struct cfg_t_ cfg_t;

void mb_iie_et__cfg_init(mb_iie_et_t *et, mb_t *mb, cfg_t *cfg, int silent);

void mb_iie_et__finish(mb_iie_et_t *et);

void mb_iie_et__clear(mb_iie_et_t *et);

#endif
