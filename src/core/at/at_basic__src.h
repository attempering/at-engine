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



/* set the initial beta as the thermostat temperature */
static void at__set_init_beta(at_t *at)
{
  /* make the initial temperature = thermostat_temp */
  double beta = at__temp_to_beta(at, at->utils->thermostat_temp);
  at_distr_domain_t *domain = at->distr->domain;

  if (beta >= domain->beta_min
   && beta <= domain->beta_max) {
    at->beta = beta;
  } else {
    at->beta = 0.5 * (domain->beta_min + domain->beta_max);
  }

  at__update_force_scale(at);

  at_utils_log__info(at->utils->log, "initial beta %g, force_scale %g\n",
      at->beta, at->force_scale);
}



static int at__cfg_init_self(
    at_t *at,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  at__set_init_beta(at);

  at->energy = 0.0;

  {
    char *fn_binary = zcom_ssm__dup(ssm, "at.dat");
    if (0 != zcom_cfg__get(cfg, &fn_binary, "at-file,at-file-binary", "%s")) {
      if (verbose) fprintf(stderr, "Info@at: assuming default at->file_binary = [%s], key: at-file-binary\n",
          fn_binary);
    }
    at->file_binary = at_utils__make_output_filename(ssm, data_dir, fn_binary);
  }

  {
    char *fn_text = zcom_ssm__dup(ssm, "at-text.dat");
    if (0 != zcom_cfg__get(cfg, &fn_text, "at-file-text", "%s")) {
      if (verbose) fprintf(stderr, "Info@at: assuming default at->file_binary = [%s], key: at-file-text\n",
          fn_text);
    }
    at->file_text = at_utils__make_output_filename(ssm, data_dir, fn_text);
  }

  at->write_text_file = AT__FALSE;
  if (0 != zcom_cfg__get(cfg, &at->write_text_file, "write-text-file", "%d")) {
    if (verbose) fprintf(stderr, "Info@at: assuming default at->write_text_file = %d, key: write-text-file\n",
        at->write_text_file);
  }

  return 0;
}



/* load settings from the configuration file
 * sys_params can be NULL, in which case, default system parameters
 * will be used */
static int at__cfg_init_low_level(at_t *at,
    zcom_cfg_t *cfg,
    const at_params_sys_t *sys_params,
    at_flags_t flags)
{
  const char *data_dir;
  zcom_ssm_t *ssm;
  at_bool_t ignore_lockfile = flags & AT__INIT_IGNORE_LOCKFILE;
  at_bool_t verbose = flags & AT__INIT_VERBOSE;

  /* initialize system parameters at->sys_params from
   * the user-provided sys_params (if not null),
   * or from the default values
   * also initialize the data directory */
  at_params_sys__init(at->sys_params, sys_params);

  /* initialize the utils objects such as manifest and trace */
  at_utils__cfg_init(at->utils, cfg,
      at->sys_params->multi_sims, at->sys_params->sim_id,
      ignore_lockfile, verbose);

  /* initialize the log file */
  at_utils_log__open_file(at->utils->log, at->sys_params->is_continuation);
  at_utils_log__push_mod(at->utils->log, "at");
  at_utils_log__info(at->utils->log, "version %lld\n", (long long) AT__VERSION);

  data_dir = at->utils->data_dir;

  ssm = at->utils->ssm;

  if (at_distr__cfg_init(at->distr, cfg, at->sys_params->boltz, verbose) != 0) {
    return -1;
  }

  if (at__cfg_init_self(at, cfg, ssm, data_dir, verbose) != 0) {
    return -1;
  }

  /* initialize the multiple-bin estimator */
  at_mb__cfg_init(at->mb, at->distr, cfg, at->sys_params->boltz, ssm, data_dir, verbose);

  /* initialize the beta driver, i.e., the Langevin equation */
  at_driver__cfg_init(at->driver, at->distr, at->mb, cfg, ssm, data_dir, verbose);

  /* initialize the energy histograms */
  at_eh__cfg_init(at->eh, at->mb, cfg, ssm, data_dir, verbose);

  return 0;
}



int at__cfg_init(at_t *at,
    zcom_cfg_t *cfg,
    const at_params_sys_t *sys_params,
    at_flags_t flags)
{
  /* load settings from the configuration file */
  at__cfg_init_low_level(at, cfg, sys_params, flags);

  at->energy = 0.0;

  /* we only load previous data if it's continuation */
  if (at->sys_params->is_continuation) {
    //fprintf(stderr, "is-cont: %d\n", at->sys_params->is_continuation);
    if (at__load_data(at) != 0) {
      fprintf(stderr, "Error@at: This simulation is started from checkpoint, while some files are missing. Will assume no previous simulation data is available.\n");
      return -1;
    }
  }

  /* open the trace file using the proper (append or write) mode
   * depending on whether it is a continuation run */
  at_utils_trace__open_file(at->utils->trace, at->sys_params->is_continuation);

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
  if ((cfg = zcom_cfg__open(cfg_filename, ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__ALLOW_DASHES)) == NULL) {
    at_utils_log__error(at->utils->log, "failed to open configuration file %s.\n", cfg_filename);
    return -1;
  }

  /* call low level function */
  if (at__cfg_init(at, cfg, sys_params, flags) != 0) {
    at_utils_log__error(at->utils->log, "error while reading configuration file %s\n",
        (cfg_filename ? cfg_filename : "NULL") );
    zcom_cfg__close(cfg);
    return -1;
  }

  at_utils_log__info(at->utils->log, "successfully loaded configuration file %s\n", cfg_filename);

  /* save a handle for `cfg`
   * Note: the `cfg` handle contains string data
   * that may still be used throughout the program,
   * So we keep the handle open and close it at the end. */
  at->cfg = cfg;

  return 0;
}



at_t *at__open(const char *cfg_filename,
    const at_params_sys_t *sys_params,
    at_flags_t flags)
{
  at_t *at;

  /* allocate memory for at_t */
  zcom_utils__exit_if ((at = (at_t *) calloc(1, sizeof(at_t))) == NULL,
      "Fatal@at: no memory for a new object of at_t\n");

  at->cfg = NULL;

  /* call low level function */
  zcom_utils__exit_if (at__init(at, cfg_filename, sys_params, flags) != 0,
      "Error@at: error while reading configuration file %s\n", cfg_filename);

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

  //fprintf(stderr, "at %p, manifest %p, fp %p (%s)\n", at, manifest, fp, manifest->file);

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
