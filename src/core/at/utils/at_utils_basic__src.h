#ifndef UTILS_BASIC__SRC_H__
#define UTILS_BASIC__SRC_H__



#include "at_utils_basic.h"
#include "log/at_utils_log.h"
#include "manifest/at_utils_manifest.h"
#include "../../zcom/zcom.h"



void at_utils__cfg_init(at_utils_t *utils, zcom_cfg_t *cfg, int silent)
{
  at_utils_log__cfg_init(utils->log, cfg, silent);
  at_utils_manifest__cfg_init(utils->manifest, cfg, silent);
}


void at_utils__finish(at_utils_t *utils)
{
  at_utils_log__finish(utils->log);
  at_utils_manifest__finish(utils->manifest);
}

#endif
