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

#ifndef AT_BASIC__SRC_H__
#define AT_BASIC__SRC_H__

#include <unistd.h>

#include "at_basic.h"
#include "at_misc.h"

#include "../zcom/zcom.h"

#include "params/at_params.h"
#include "utils/at_utils.h"
#include "distr/at_distr.h"
#include "mb/at_mb.h"
#include "driver/at_driver.h"
#include "eh/at_eh.h"



static void at__set_init_beta(at_t *at)
{
  /* make the initial temperature = temp_thermostat */
  double beta = at__temp_to_beta(at, at->utils->temp_thermostat);
  at_distr_domain_t *domain = at->distr->domain;

  if (beta >= domain->beta_min
   && beta <= domain->beta_max) {
    at->beta = beta;
  } else {
    at->beta = 0.5 * (domain->beta_min + domain->beta_max);
  }
  fprintf(stderr, "Info@at: initial beta %g\n", at->beta);
}



int at__cfg_init(at_t *at,
    zcom_cfg_t *cfg,
    const at_params_sys_t *sys_params,
    at_flags_t flags)
{
  const char *data_dir;
  zcom_ssm_t *ssm;
  at_bool_t verbose = flags & AT__INIT_VERBOSE;

  at_params_sys__init(at->sys_params, sys_params, verbose);

  at_utils__cfg_init(at->utils, cfg, at->sys_params->data_dir, verbose);

  data_dir = at->sys_params->data_dir;
  ssm = at->utils->ssm;

  if (at_distr__cfg_init(at->distr, cfg, at->sys_params->boltz, verbose) != 0) {
    return -1;
  }

  at__set_init_beta(at);

  /* handle for multiple-bin estimator */
  at_mb__cfg_init(at->mb, at->distr, cfg, at->sys_params->boltz, ssm, data_dir, verbose);

  at_driver__cfg_init(at->driver, at->distr, at->mb, cfg, ssm, data_dir, verbose);

  /* energy histogram */
  at_eh__cfg_init(at->eh, at->mb, cfg, ssm, data_dir, verbose);


  /* we only load previous data if it's continuation */
  if (at__load_data(at, at->sys_params->is_continuation) != 0) {
    fprintf(stderr, "Warning@at: This simulation is started from checkpoint, while some files are missing. Will assume no previous simulation data is available.\n");
  }

  // open the trace file using the proper mode
  at_utils_trace__open_file(at->utils->trace, at->sys_params->is_continuation);

  at->energy = 0.0;

  return 0;
}



int at__init(at_t *at,
    const char *cfg_filename,
    const at_params_sys_t *sys_params,
    at_flags_t flags)
{
  zcom_cfg_t *cfg = NULL;

  at->cfg = NULL;

  if (cfg_filename == NULL) {
    return -1;
  }

  //fprintf(stderr, "Debug@at: at__init() from %s\n", cfg_filename);

  /* open configuration file */
  if ((cfg = zcom_cfg__open(cfg_filename, ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__IGNORE_DASHES)) == NULL) {
    fprintf(stderr, "\rError@at: cannot open configuration file %s.\n", cfg_filename);
    return -1;
  }

  /* call low level function */
  if (at__cfg_init(at, cfg, sys_params, flags) != 0) {
    fprintf(stderr, "\rError@at: error while reading configuration file %s\n",
        (cfg_filename ? cfg_filename : "NULL") );
    zcom_cfg__close(cfg);
    return -1;
  }

  fprintf(stderr, "Info@at: successfully loaded configuration file %s\n", cfg_filename);

  at->cfg = cfg;

  return 0;
}



at_t *at__open(const char *cfg_filename,
    const at_params_sys_t *sys_params,
    at_flags_t flags)
{
  at_t *at;

  /* allocate memory for at_t */
  zcom_util__exit_if ((at = (at_t *) calloc(1, sizeof(at_t))) == NULL,
      "Fatal: no memory for a new object of at_t\n");

  at->cfg = NULL;

  /* call low level function */
  zcom_util__exit_if (at__init(at, cfg_filename, sys_params, flags) != 0,
    "at_t: error while reading configuration file %s\n", cfg_filename);

  return at;
}




void at__finish(at_t *at, at_flags_t flags)
{
  at_eh__finish(at->eh);

  at_driver__finish(at->driver);

  at_mb__finish(at->mb);

  at_distr__finish(at->distr);

  at_utils__finish(at->utils);

  if (flags & AT__FINISH_CLOSE_CFG) {
    //fprintf(stderr, "at->cfg %p\n", at->cfg);
    if (at->cfg != NULL) {
      zcom_cfg__close(at->cfg);
    }
  }

  memset(at, 0, sizeof(*at));
}


void at__close(at_t *at)
{
  at__finish(at, AT__FINISH_CLOSE_CFG);
  free(at);
}


int at__manifest(at_t *at)
{
  at_utils_manifest_t *manifest = at->utils->manifest;

  at_utils_manifest__open_file(manifest);

  //FILE *fp = at_utils_manifest__open_file(manifest);

  //fprintf(stderr, "at %p, manifest %p, fp %p (%s)\n", at, manifest, fp, manifest->filename);

  at_utils_manifest__print_double(manifest, at->beta, "beta", NULL);

  at_utils_manifest__print_double(manifest, at->energy, "energy", NULL);

  at_utils__manifest(at->utils);

  at_distr__manifest(at->distr, manifest);

  at_mb__manifest(at->mb, manifest);

  at_driver__manifest(at->driver, manifest);

  at_eh__manifest(at->eh, manifest);

  //fprintf(stderr, "fp %p %p, error %d\n", fp, manifest->fp, ferror(fp)); getchar();

  at_utils_manifest__close_file(manifest);

  return 0;
}



#endif
