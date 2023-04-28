#ifndef AT_LANGEVIN_MOVE_UTILS__SRC_H__
#define AT_LANGEVIN_MOVE_UTILS__SRC_H__



#include "at_langevin_move_basic.h"



double at_langevin_move__get_acceptance_ratio(at_langevin_t *langevin)
{
  if (langevin->total > 0) {
    return 1 - langevin->rejects/langevin->total; 
  } else {
    return 0;
  }
}



#endif
