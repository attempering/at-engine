#ifndef UTILS_BASIC_H__
#define UTILS_BASIC_H__


#include "at_utils__def.h"

#include "../../zcom/zcom.h"



void at_utils__cfg_init(at_utils_t *utils, zcom_cfg_t *cfg, int silent);

void at_utils__finish(at_utils_t *utils);


#endif
