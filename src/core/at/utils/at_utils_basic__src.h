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
#include "conf/at_utils_conf.h"
#include "lockfile/at_utils_lockfile.h"
#include "manifest/at_utils_manifest.h"
#include "trace/at_utils_trace.h"


/* initialize the data directory */
static void at_utils__init_data_dir(at_utils_t *utils,
    zcom_cfg_t *cfg,
    at_bool_t append_sim_id_to_data_dir, int sim_id,
    at_bool_t verbose)
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



static void at_utils__conf_init_self(at_utils_t *utils)
{
  /* initialize the thermostat temperature */
  at_utils_conf__get_double(utils->conf,
      "T0,thermostat-temp",
      &utils->thermostat_temp, 300.0,
      "utils->thermostat_temp");
}


void at_utils__cfg_init(
    at_utils_t *utils,
    zcom_ssm_t *ssm,
    zcom_cfg_t *cfg,
    at_bool_t is_continuation,
    at_bool_t append_sim_id_to_data_dir,
    int sim_id,
    at_bool_t ignore_lockfile,
    at_bool_t verbose)
{
  utils->ssm = ssm;

  at_utils__init_data_dir(utils, cfg, append_sim_id_to_data_dir, sim_id, verbose);

  /* initialize the log file */
  at_utils_log__cfg_init(utils->log, cfg, utils->ssm, utils->data_dir, verbose);
  at_utils_log__open_file(utils->log, is_continuation);
  at_utils_log__push_mod(utils->log, "at");

  at_utils_conf__init(utils->conf, cfg, ssm, utils->data_dir, utils->log, verbose);

  at_utils__conf_init_self(utils);

  at_utils_lockfile__init(utils->lockfile, ignore_lockfile,
      utils->log, utils->ssm, utils->data_dir, verbose);

  at_utils_manifest__conf_init(utils->manifest, utils->conf);

  at_utils_trace__conf_init(utils->trace, utils->conf);
  at_utils_trace__open_file(utils->trace, is_continuation);

  utils->ready = AT__TRUE;
}



void at_utils__finish(at_utils_t *utils)
{
  at_utils_trace__finish(utils->trace);
  at_utils_manifest__finish(utils->manifest);
  at_utils_lockfile__finish(utils->lockfile);

  at_utils_conf__finish(utils->conf);
  at_utils_log__finish(utils->log);

  utils->ready = AT__FALSE;

}



void at_utils__manifest(at_utils_t *utils)
{
  at_utils_manifest__manifest(utils->manifest);

  at_utils_trace__manifest(utils->trace, utils->manifest);

  at_utils_log__manifest(utils->log, utils->manifest);


  at_utils_manifest__push_mod(utils->manifest, "at.utils");

  at_utils_manifest__print_double(utils->manifest, utils->thermostat_temp, "thermostat_temp", "thermostat-temp");

  at_utils_manifest__pop_mod(utils->manifest);
}


#endif
