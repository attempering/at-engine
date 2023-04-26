#ifndef AT__MB_IIE__BASIC_SRC_H__
#define AT__MB_IIE__BASIC_SRC_H__


#include "mb_iie_basic.h"

#include "zerofiller/mb_iie_zerofiller.h"
#include "lr/mb_iie_lr.h"
#include "et/mb_iie_et.h"
#include "gridvals/mb_iie_gridvals.h"

#include "../mb__def.h"
#include "../../zcom/cfg/cfg.h"


void mb_iie__cfg_init(mb_iie_t *iie, mb_t *mb,
    cfg_t *cfg, int silent)
{
  iie->n = mb->n;
  iie->flags = mb->flags;
  iie->win = mb->win;
  iie->accum = mb->accum;

  mb_iie_zerofiller__init(iie->zf, mb);
  mb_iie_lr__cfg_init(iie->lr, iie->zf, mb, cfg, silent);
  mb_iie_et__cfg_init(iie->et, mb, cfg, silent);
  mb_iie_gridvals__cfg_init(iie->gridvals, mb, cfg, silent);
}


void mb_iie__finish(mb_iie_t *iie)
{
  mb_iie_lr__finish(iie->lr);
  mb_iie_zerofiller__finish(iie->zf);
  mb_iie_et__finish(iie->et);
  mb_iie_gridvals__finish(iie->gridvals);
}


void mb_iie__clear(mb_iie_t *iie)
{
  mb_iie_et__clear(iie->et);
  mb_iie_gridvals__clear(iie->gridvals);
}


void mb_iie__manifest(const mb_iie_t *iie, FILE *fp, int arrmax)
{
}

#endif
