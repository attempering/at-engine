#ifndef AT_MB__SRC_H__
#define AT_MB__SRC_H__



/* external dependencies */
#include "../../zcom/zcom__src.h"


/* simple accumulator */
#include "sm/at_mb_sm__src.h"

/* beta distribution functions */
#include "betadist/at_mb_betadist__src.h"

/* shrinking rate of the adaptive averaging scheme */
#include "shk/at_mb_shk__src.h"

/* window set-up functions */
#include "win/at_mb_win__src.h"

/* accumulators */
#include "accum/at_mb_accum__src.h"

#include "at_mb_basic__src.h"

#include "at_mb_utils__src.h"

/* mb_io_binary__src.h is included within */
#include "io/at_mb_io__src.h"

/* integral-identity estimators */
#include "iie/at_mb_iie__src.h"

#endif
