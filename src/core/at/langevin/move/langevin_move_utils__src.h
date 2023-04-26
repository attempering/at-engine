#ifndef AT__LANGEVIN_MOVE_UTILS__SRC_H__
#define AT__LANGEVIN_MOVE_UTILS__SRC_H__



#include "langevin_move_basic.h"



double langevin_move__get_acceptance_ratio(langevin_t *langevin)
{
  if (langevin->total > 0) {
    return 1 - langevin->rejects/langevin->total; 
  } else {
    return 0;
  }
}



#endif
