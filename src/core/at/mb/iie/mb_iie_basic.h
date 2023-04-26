#ifndef AT__MB_IIE__BASIC_H__
#define AT__MB_IIE__BASIC_H__

#include "mb_iie__def.h"

#include "mb_iie_basic.h"

void at_mb_iie__cfg_init(at_mb_iie_t *iie, at_mb_t *mb, zcom_cfg_t *cfg, int silent);

void at_mb_iie__finish(at_mb_iie_t *iie);

void at_mb_iie__clear(at_mb_iie_t *iie);

void at_mb_iie__manifest(const at_mb_iie_t *iie, FILE *fp, int arrmax);


#endif
