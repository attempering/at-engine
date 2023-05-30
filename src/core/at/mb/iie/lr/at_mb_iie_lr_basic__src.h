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
void at_mb_iie_lr__cfg_init(at_mb_iie_lr_t *lr, at_mb_iie_zerofiller_t *zf, at_mb_t *mb, zcom_cfg_t *cfg, at_bool_t verbose)
{
  lr->mb = mb;
  lr->zerofiller = zf;

  /* frac_min: minimum acceptable coefficient during left/right combination */
  lr->frac_min = 0.0;
  if (0 != zcom_cfg__get(cfg, &lr->frac_min, "mbest-fracmin,mbest-frac-min,mbest-min-frac", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default mb->iie->lr->frac_min = 0.0, key: mbest-min-frac\n");
  }

  lr->min_size = AT_MB_IIE_LR__DEFAULT_MIN_SIZE;
  if (0 != zcom_cfg__get(cfg, &lr->min_size, "mbest-min-size", "%lf")) {
    if (verbose) {
      fprintf(stderr, "Info@at: assuming default mb->iie->lr->min_size = %lf, key: mbest-min-size\n",
          AT_MB_IIE_LR__DEFAULT_MIN_SIZE);
    }
  }

  /* cv_shift_max: maximum fraction for shifting energy fluctuations
   * If the heat capacity cv is monotonic, it should be 0.0,
   * For the Ising model, it can restrain the magnitude */
  lr->cv_shift_max = 1.0;
  if (0 != zcom_cfg__get(cfg, &lr->cv_shift_max, "mbest-cvshiftmax,mbest-cv-shift-max,mbest-max-cv-shift", "%lf")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default mb->iie->lr->cv_shift_max = 1.0, key: mbest-max-cv-shift\n");
  }

}



void at_mb_iie_lr__finish(at_mb_iie_lr_t *lr)
{
  lr->zerofiller = NULL;
}


void at_mb_iie_lr__manifest(const at_mb_iie_lr_t *lr, at_utils_manifest_t *manifest)
{
  /* minimal allowable coefficient during left/right combination */
  at_utils_manifest__print_double(manifest, lr->frac_min, "mb->iie->lr->frac_min", "mbest-min-frac");

  at_utils_manifest__print_double(manifest, lr->min_size, "mb->iie->lr->min_size", "mbest-min-size");

  /* maximal fraction for shift energy fluct. if cv is monotonic, it should be 0.0, for ising model, it can restrain the magnitude */
  at_utils_manifest__print_double(manifest, lr->cv_shift_max, "mb->iie->lr->cv_shift_max", "mbest-max-cv-shift");
}

#endif

