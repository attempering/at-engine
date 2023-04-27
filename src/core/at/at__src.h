#ifndef AT__AT__SRC_H__
#define AT__AT__SRC_H__



/* this header needs to be placed before zcom modules
 * so that we can determine whether `real` should be defined or not */
#include "context/context.h"



/* source code of zcom modules */
#ifdef ADAPTIVE_TEMPERING_FOR_GROMACS
  #define ZCOM__HAVE_REAL 1
#endif

#include "zcom/def/def.h"
#include "zcom/util/util__src.h"
#include "zcom/endn/endn__src.h"
#include "zcom/ssm/ssm__src.h"
#include "zcom/opt/opt__src.h"
#include "zcom/cfg/cfg__src.h"
#include "zcom/rng/rng__src.h"
#include "zcom/log/log__src.h"


#include "utils/utils__src.h"


/* include the source code of sub-modules */
#include "mb/mb__src.h"
#include "eh/eh__src.h"
#include "langevin/langevin__src.h"

#include "at_basic__src.h"
#include "at_utils__src.h"


#endif
