#ifndef AT__MB_IIE__BASIC_H__
#define AT__MB_IIE__BASIC_H__

#include "mb_iie__def.h"

#include "mb_iie_basic.h"

void mb_iie__cfg_init(mb_iie_t *iie, mb_t *mb, cfg_t *cfg, int silent);

void mb_iie__finish(mb_iie_t *iie);

void mb_iie__clear(mb_iie_t *iie);

void mb_iie__manifest(const mb_iie_t *iie, FILE *fp, int arrmax);


#endif
