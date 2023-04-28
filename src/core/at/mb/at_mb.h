/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT_MB_H__
#define AT_MB_H__

/* definition of at_mb_t */
#include "at_mb__def.h"

/* simple accumulator */
#include "sm/at_mb_sm.h"

/* beta distribution functions */
#include "betadist/at_mb_betadist.h"

/* shrinking rate of the adaptive averaging scheme */
#include "shk/at_mb_shk.h"

/* window set-up functions */
#include "win/at_mb_win.h"

/* accumulators */
#include "accum/at_mb_accum.h"

/* basic routines, such as zcom_cfg__open()/close() */
#include "at_mb_basic.h"

/* miscellaneous routines */
#include "at_mb_utils.h"

/* IO routines */
#include "io/at_mb_io.h"

/* integral-identity-based estimators */
#include "iie/at_mb_iie.h"

#endif
