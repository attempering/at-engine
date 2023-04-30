/* 
 * Copyright (C) 2010-2023  AT-Engine Developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef AT_MB_IIE_BASIC__SRC_H__
#define AT_MB_IIE_BASIC__SRC_H__


#include "at_mb_iie_basic.h"

#include "zerofiller/at_mb_iie_zerofiller.h"
#include "lr/at_mb_iie_lr.h"
#include "et/at_mb_iie_et.h"
#include "gridvals/at_mb_iie_gridvals.h"

#include "../at_mb__def.h"
#include "../../../zcom/zcom.h"


void at_mb_iie__cfg_init(at_mb_iie_t *iie, at_mb_t *mb,
    zcom_cfg_t *cfg, at_bool_t verbose)
{
  iie->n = mb->distr->domain->n;
  iie->flags = mb->flags;
  iie->win = mb->win;
  iie->accum = mb->accum;

  at_mb_iie_zerofiller__init(iie->zf, mb);
  at_mb_iie_lr__cfg_init(iie->lr, iie->zf, mb, cfg, verbose);
  at_mb_iie_et__cfg_init(iie->et, mb, cfg, verbose);
  at_mb_iie_gridvals__cfg_init(iie->gridvals, mb, cfg, verbose);
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


void at_mb_iie__manifest(const at_mb_iie_t *iie, at_utils_manifest_t *manifest)
{
  at_mb_iie_lr__manifest(iie->lr, manifest);
  at_mb_iie_et__manifest(iie->et, manifest);
}

#endif
