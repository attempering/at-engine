/* 
 * Copyright (C) 2010-2023  AT-Engine Developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef AT_UTILS_BASIC__SRC_H__
#define AT_UTILS_BASIC__SRC_H__



#include "at_utils_basic.h"
#include "log/at_utils_log.h"
#include "manifest/at_utils_manifest.h"
#include "../../zcom/zcom.h"



void at_utils__cfg_init(at_utils_t *utils, zcom_cfg_t *cfg,
    int isuffix, int silent)
{
  sprintf(utils->data_dir, "atdata%d", isuffix);
  printf("datadir: %s\n", utils->data_dir);

  utils->ssm = zcom_ssm__open();
  at_utils_manifest__cfg_init(utils->manifest, cfg, utils->ssm, utils->data_dir, silent);
  at_utils_log__cfg_init(utils->log, cfg, utils->ssm, utils->data_dir, silent);
  utils->inited = 1;
}


void at_utils__finish(at_utils_t *utils)
{
  at_utils_log__finish(utils->log);
  at_utils_manifest__finish(utils->manifest);
  if (utils->inited) {
    zcom_ssm__close(utils->ssm);
    utils->ssm = NULL;
  }
}

void at_utils__manifest(at_utils_t *utils)
{
  at_utils_manifest__manifest(utils->manifest);
  at_utils_log__manifest(utils->log, utils->manifest);
}

#endif
