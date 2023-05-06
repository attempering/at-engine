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



/* load previous data */
int at__load_data(at_t *at, at_bool_t is_continuation)
{
  at_mb_t *mb = at->mb;
  int load_data;

  if (!is_continuation) { /* initial run */
    return 0;
  }

  load_data = is_continuation;
  if (load_data) {
     /* read previous at_mb_t data */
    if (at_mb__read(mb, at->driver->langevin, &at->beta) != 0) {
      fprintf(stderr, "cannot load mb data from %s\n", mb->av_file);
      return 1;
    }

    at_mb__write_ze_file(mb, "ze_init.dat");

    /* read previous energy histogram data */
    if (at_eh__read(at->eh) != 0) {
      fprintf(stderr, "cannot load energy histogram from %s\n", at->eh->file);
      return 2;
    }
  }

  fprintf(stderr, "successfully load previous data\n");

  return 0;
}



static void at__set_init_beta(at_t *at)
{
  /* make the initial temperature = temp_thermostat */
  double beta = at__temp_to_beta(at, at->utils->temp_thermostat);

  if (beta >= at->distr->domain->bmin
   && beta <= at->distr->domain->bmax) {
    at->beta = beta;
  } else {
    at->beta = 0.5 * (at->distr->domain->bmin + at->distr->domain->bmax);
  }
  fprintf(stderr, "initial beta %g\n", at->beta);
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
    fprintf(stderr, "Warning: This simulation is started from checkpoint, while some files are missing. Will assume no previous simulation data is available.\n");
  }

  // no need to do so, the log file will be open upon the first time of writing
  //if (flags & AT__INIT_OPENLOG) {
  //  at_utils_log__open_file(at->utils->log);
  //}

  at->energy = 0.0;

  return 0;
}



int at__init(at_t *at,
    const char *cfg_filename,
    const at_params_sys_t *sys_params,
    at_flags_t flags)
{
  zcom_cfg_t *cfg = NULL;

  if (cfg_filename != NULL) {
    /* open configuration file */
    zcom_util__exit_if((cfg = zcom_cfg__open(cfg_filename)) == NULL,
        "at_t: cannot open configuration file %s.\n", cfg_filename);
  }

  /* call low level function */
  zcom_util__exit_if (at__cfg_init(at, cfg, sys_params, flags) != 0,
      "at_t: error while reading configuration file %s\n",
      (cfg_filename ? cfg_filename : "NULL") );

  if (cfg_filename != NULL) {
    fprintf(stderr, "Successfully loaded configuration file %s\n", cfg_filename);
  }

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

  /* call low level function */
  zcom_util__exit_if (at__init(at, cfg_filename, sys_params, flags) != 0,
    "at_t: error while reading configuration file %s\n", cfg_filename);

  return at;
}




void at__finish(at_t *at)
{
  at_eh__finish(at->eh);

  at_driver__finish(at->driver);

  at_mb__finish(at->mb);

  at_distr__finish(at->distr);

  at_utils__finish(at->utils);

  memset(at, 0, sizeof(*at));
}


void at__close(at_t *at)
{
  at__finish(at);
  free(at);
}


int at__manifest(at_t *at)
{
  at_utils_manifest_t *manifest = at->utils->manifest;

  FILE *fp = at_utils_manifest__open_file(manifest);

  //fprintf(stderr, "at %p, manifest %p, fp %p (%s)\n", at, manifest, fp, manifest->filename);

  at_utils__manifest(at->utils);

  at_distr__manifest(at->distr, manifest);

  at_mb__manifest(at->mb, manifest);

  if (at->eh->mode != 0) {
    at_eh__manifest(at->eh, manifest);
  }

  at_driver__manifest(at->driver, manifest);

  fprintf(fp, "at->beta: double, %g\n", at->beta);

  fprintf(fp, "at->energy: double, %g\n", at->energy);

  //fprintf(stderr, "fp %p %p, error %d\n", fp, manifest->fp, ferror(fp)); getchar();

  at_utils_manifest__close_file(manifest);

  return 0;
}



#endif
