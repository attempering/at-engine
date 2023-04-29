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

#ifndef AT_MB_IIE__DEF_H__
#define AT_MB_IIE__DEF_H__



#include "zerofiller/at_mb_iie_zerofiller__def.h"

#include "lr/at_mb_iie_lr__def.h"

#include "et/at_mb_iie_et__def.h"

#include "gridvals/at_mb_iie_gridvals__def.h"



typedef struct at_mb_iie_t_ {

  int n; /* == distr->domain->n */
  unsigned flags; /* == mb->flags */

  at_mb_iie_lr_t lr[1];

  at_mb_iie_zerofiller_t zf[1];

  at_mb_iie_et_t et[1];

  at_mb_iie_gridvals_t gridvals[1];

  at_mb_win_t *win; /* reference to mb->win */
  at_mb_accum_t *accum; /* reference to mb->accum */

} at_mb_iie_t;



#endif
