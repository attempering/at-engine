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

#ifndef AT_MB_IIE_LR_BASIC__SRC_H__
#define AT_MB_IIE_LR_BASIC__SRC_H__


#include "at_mb_iie_lr.h"

/* implementation dependent headers */
#include "../../at_mb_basic.h"
#include "../../../../zcom/zcom.h"
#include "../../accum/at_mb_accum.h"
#include "../zerofiller/at_mb_iie_zerofiller.h"



/* global initialization */
void at_mb_iie_lr__conf_init(at_mb_iie_lr_t *lr, at_mb_iie_zerofiller_t *zf, at_mb_t *mb,
    at_utils_conf_t *conf)
{
  lr->mb = mb;
  lr->zerofiller = zf;

  at_utils_conf__push_mod(conf, "at.mb.iie.lr");

  /* frac_min: minimum acceptable coefficient during left/right combination */
  at_utils_conf__get_double(conf,
      "mbest_fracmin,mbest-frac-min,mb-frac-min,mb-min-frac",
      &lr->frac_min, 0.0,
      "frac_min");

  at_utils_conf__get_double(conf,
      "mbest_min_size,mb-min-size",
      &lr->min_size, AT_MB_IIE_LR__DEFAULT_MIN_SIZE,
      "min_size");

  /* cv_shift_max: maximum fraction for shifting energy fluctuations
   * If the heat capacity cv is monotonic, it should be 0.0,
   * For the Ising model, it can restrain the magnitude */
  at_utils_conf__get_double(conf,
      "mbest_cvshiftmax,mbest-cv-shift-max,mb-cv-shift-max,mb-max-cv-shift",
      &lr->cv_shift_max, 1.0,
      "cv_shift_max");

  at_utils_conf__pop_mod(conf);

}



void at_mb_iie_lr__finish(at_mb_iie_lr_t *lr)
{
  lr->zerofiller = NULL;
}


void at_mb_iie_lr__manifest(const at_mb_iie_lr_t *lr, at_utils_manifest_t *manifest)
{
  /* minimal allowable coefficient during left/right combination */
  at_utils_manifest__print_double(manifest, lr->frac_min, "mb->iie->lr->frac_min", "mb-min-frac");

  at_utils_manifest__print_double(manifest, lr->min_size, "mb->iie->lr->min_size", "mb-min-size");

  /* maximal fraction for shift energy fluct. if cv is monotonic, it should be 0.0, for Ising model, it can restrain the magnitude */
  at_utils_manifest__print_double(manifest, lr->cv_shift_max, "mb->iie->lr->cv_shift_max", "mb-max-cv-shift");
}

#endif

