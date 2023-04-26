/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT__MB_H__
#define AT__MB_H__

/* definition of at_mb_t */
#include "mb__def.h"

/* simple accumulator */
#include "sm/mb_sm.h"

/* beta distribution functions */
#include "betadist/mb_betadist.h"

/* shrinking rate of the adaptive averaging scheme */
#include "shk/mb_shk.h"

/* window set-up functions */
#include "win/mb_win.h"

/* accumulators */
#include "accum/mb_accum.h"

/* basic routines, such as zcom_cfg__open()/close() */
#include "mb_basic.h"

/* miscellaneous routines */
#include "mb_utils.h"

/* IO routines */
#include "io/mb_io.h"

/* integral-identity-based estimators */
#include "iie/mb_iie.h"

#endif
