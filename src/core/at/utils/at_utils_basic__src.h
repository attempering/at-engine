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
#include "lockfile/at_utils_lockfile.h"
#include "manifest/at_utils_manifest.h"
#include "trace/at_utils_trace.h"
#include "../../zcom/zcom.h"



void at_utils__cfg_init(at_utils_t *utils, zcom_cfg_t *cfg,
    at_bool_t append_sim_id_to_data_dir, int sim_id,
    at_bool_t ignore_lockfile, at_bool_t verbose)
{
  utils->ssm = zcom_ssm__open();

  /* initialize the thermostat temperature */
  utils->thermostat_temp = 300.0;
  if (zcom_cfg__get(cfg, &utils->thermostat_temp, "T0,thermostat-temp", "%lf") != 0) {
    if (verbose) fprintf(stderr, "Info@at.utils: assuming default utils->thermostat_temp = 300.0, key: thermostat-temp\n");
  }

  /* initialize the data directory */
  {
    char *data_dir = zcom_ssm__dup(utils->ssm, "atdata");

    if (zcom_cfg__get(cfg, &data_dir, "data-dir", "%s") != 0) {
      if (verbose) fprintf(stderr, "Info@at.utils: assuming default utils->data_dir = %s, key: data-dir\n",
          data_dir);
    }

    if (append_sim_id_to_data_dir) {
      char sim_id_str[64] = "";

      snprintf(sim_id_str, 64, "%d", sim_id);

      zcom_ssm__concat(utils->ssm, &data_dir, sim_id_str);
    }

    utils->data_dir = data_dir;
  }

  at_utils_lockfile__init(utils->lockfile, ignore_lockfile, utils->ssm, utils->data_dir, verbose);

  at_utils_manifest__cfg_init(utils->manifest, cfg, utils->ssm, utils->data_dir, verbose);

  at_utils_trace__cfg_init(utils->trace, cfg, utils->ssm, utils->data_dir, verbose);

  utils->inited = 1;
}



void at_utils__finish(at_utils_t *utils)
{
  at_utils_trace__finish(utils->trace);
  at_utils_manifest__finish(utils->manifest);
  at_utils_lockfile__finish(utils->lockfile);

  if (utils->inited) {
    zcom_ssm__close(utils->ssm);
    utils->ssm = NULL;
    utils->inited = 0;
  }

}



void at_utils__manifest(at_utils_t *utils)
{
  at_utils_manifest__manifest(utils->manifest);

  at_utils_trace__manifest(utils->trace, utils->manifest);

  at_utils_manifest__print_double(utils->manifest, utils->thermostat_temp, "utils->thermostat_temp", "thermostat-temp");
}


#endif
