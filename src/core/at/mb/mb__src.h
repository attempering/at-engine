#ifndef AT__MB__SRC_H__
#define AT__MB__SRC_H__



/* external dependencies */
#include "../zcom/util/util__src.h"
#include "../zcom/endn/endn__src.h"
#include "../zcom/ssm/ssm__src.h"
#include "../zcom/opt/opt__src.h"
#include "../zcom/cfg/cfg__src.h"
#include "../sm/sm__src.h"


/* beta distribution functions */
#include "betadist/mb_betadist__src.h"

/* shrinking rate of the adaptive averaging scheme */
#include "shk/mb_shk__src.h"

/* window set-up functions */
#include "win/mb_win__src.h"

/* accumulators */
#include "accum/mb_accum__src.h"

#include "mb_basic__src.h"

#include "mb_utils__src.h"

/* mb_io_binary__src.h is included within */
#include "io/mb_io__src.h"

#include "zerofiller/mb_zerofiller__src.h"

#include "integrator/mb_integrator__src.h"

/* integral-identity estimators */
#include "iie/mb_iie__src.h"

#endif
