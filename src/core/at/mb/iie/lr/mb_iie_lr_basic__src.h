#ifndef AT__MB_IIE_LR_BASIC__SRC_H__
#define AT__MB_IIE_LR_BASIC__SRC_H__


#include "mb_iie_lr.h"

/* implementation dependent headers */
#include "../../mb_basic.h"
#include "../../../zcom/cfg/cfg.h"
#include "../../accum/mb_accum.h"
#include "../zerofiller/mb_iie_zerofiller.h"



/* global initialization */
void at_mb_iie_lr__cfg_init(at_mb_iie_lr_t *lr, at_mb_iie_zerofiller_t *zf, at_mb_t *mb, zcom_cfg_t *cfg, int silent)
{
  lr->mb = mb;
  lr->zerofiller = zf;

  /* frac_min: minimum acceptable coefficient during left/right combination */
  lr->frac_min = 0.0;
  if (0 != zcom_cfg__get(cfg, &lr->frac_min, "mbest_fracmin", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: mb->iie->lr->frac_min = 0.0, key: mbest_fracmin\n");
  }


  /* cv_shift_max: maximum fraction for shifting energy fluctuations
   * If the heat capacity cv is monotonic, it should be 0.0,
   * For the Ising model, it can restrain the magnitude */
  lr->cv_shift_max = 1.0;
  if (0 != zcom_cfg__get(cfg, &lr->cv_shift_max, "mbest_cvshiftmax", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: mb->iie->lr->cv_shift_max = 1.0, key: mbest_cvshiftmax\n");
  }


  lr->min_size = MB_IIE_LR__DEFAULT_MIN_SIZE;
  if (0 != zcom_cfg__get(cfg, &lr->min_size, "mbest_min_size", "%lf")) {
    if (!silent) {
      fprintf(stderr, "assuming default: mb->iie->lr->min_size = %lf, key: mbest_min_size\n",
          MB_IIE_LR__DEFAULT_MIN_SIZE);
    }
  }

}



void at_mb_iie_lr__finish(at_mb_iie_lr_t *lr)
{
}




#endif

