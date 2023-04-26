#ifndef AT__MB_IIE__BASIC_SRC_H__
#define AT__MB_IIE__BASIC_SRC_H__


#include "mb_iie_basic.h"

#include "zerofiller/mb_iie_zerofiller.h"
#include "lr/mb_iie_lr.h"
#include "et/mb_iie_et.h"
#include "gridvals/mb_iie_gridvals.h"

#include "../mb__def.h"
#include "../../zcom/cfg/cfg.h"


void at_mb_iie__cfg_init(at_mb_iie_t *iie, at_mb_t *mb,
    zcom_cfg_t *cfg, int silent)
{
  iie->n = mb->n;
  iie->flags = mb->flags;
  iie->win = mb->win;
  iie->accum = mb->accum;

  at_mb_iie_zerofiller__init(iie->zf, mb);
  at_mb_iie_lr__cfg_init(iie->lr, iie->zf, mb, cfg, silent);
  at_mb_iie_et__cfg_init(iie->et, mb, cfg, silent);
  at_mb_iie_gridvals__cfg_init(iie->gridvals, mb, cfg, silent);
}


void at_mb_iie__finish(at_mb_iie_t *iie)
{
  at_mb_iie_lr__finish(iie->lr);
  at_mb_iie_zerofiller__finish(iie->zf);
  at_mb_iie_et__finish(iie->et);
  at_mb_iie_gridvals__finish(iie->gridvals);
}


void at_mb_iie__clear(at_mb_iie_t *iie)
{
  at_mb_iie_et__clear(iie->et);
  at_mb_iie_gridvals__clear(iie->gridvals);
}


void at_mb_iie__manifest(const at_mb_iie_t *iie, FILE *fp, int arrmax)
{
}

#endif
