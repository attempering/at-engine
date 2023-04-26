#ifndef AT__MB_IIE__DEF_H__
#define AT__MB_IIE__DEF_H__



#include "zerofiller/mb_iie_zerofiller__def.h"

#include "lr/mb_iie_lr__def.h"

#include "et/mb_iie_et__def.h"

#include "gridvals/mb_iie_gridvals__def.h"



typedef struct at_mb_iie_t_ {

  int n; /* == mb->n */
  unsigned flags; /* == mb->flags */

  at_mb_iie_lr_t lr[1];

  at_mb_iie_zerofiller_t zf[1];

  at_mb_iie_et_t et[1];

  at_mb_iie_gridvals_t gridvals[1];

  at_mb_win_t *win; /* reference to mb->win */
  at_mb_accum_t *accum; /* reference to mb->accum */

} at_mb_iie_t;



#endif
