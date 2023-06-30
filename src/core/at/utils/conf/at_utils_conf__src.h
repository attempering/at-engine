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

#ifndef AT_UTILS_CONF__SRC_H__
#define AT_UTILS_CONF__SRC_H__

#include "at_utils_conf.h"

#include "../../../zcom/zcom.h"
#include "../at_utils_misc.h"



void at_utils_conf__init(at_utils_conf_t *conf,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_utils_log_t *log,
    at_bool_t verbose)
{
  conf->cfg = cfg;
  conf->ssm = ssm;
  conf->data_dir = data_dir;
  conf->log = log;
  conf->verbose = verbose;

  conf->ready = AT__TRUE;
}



void at_utils_conf__finish(at_utils_conf_t *conf)
{
  conf->ready = AT__FALSE;
}


void at_utils_conf__push_mod(at_utils_conf_t *conf, const char *mod)
{
  at_utils_log__push_mod(conf->log, mod);
}


void at_utils_conf__pop_mod(at_utils_conf_t *conf)
{
  at_utils_log__pop_mod(conf->log);
}


int at_utils_conf__get_int(
    at_utils_conf_t *conf,
    const char *keys,
    int *var,
    int val_def,
    const char *name)
{
  int tmp;

  *var = val_def;

  if (zcom_cfg__get(conf->cfg, &tmp, keys, "%d") != 0) {

    if (conf->verbose) {
      at_utils_log__info(conf->log, "assuming default %s = %d, key: %s\n",
          name, val_def, keys);
    }
  }

  *var = tmp;

  return 0;
}


int at_utils_conf__get_double(
    at_utils_conf_t *conf,
    const char *keys,
    double *var,
    double val_def,
    const char *name)
{
  double tmp;

  *var = val_def;

  if (zcom_cfg__get(conf->cfg, &tmp, keys, "%lf") != 0) {
    if (conf->verbose) {
      at_utils_log__info(conf->log, "assuming default %s = %g, key: %s\n",
          name, val_def, keys);
    }
  }

  *var = tmp;

  return 0;
}



int at_utils_conf__get_str(
    at_utils_conf_t *conf,
    const char *keys,
    char **var,
    const char *val_def,
    const char *name)
{
  char *tmp = zcom_ssm__dup(conf->ssm, val_def);

  if (zcom_cfg__get(conf->cfg, &tmp, keys, "%s") != 0) {
    if (conf->verbose) {
      at_utils_log__info(conf->log, "assuming default %s->%s = \"%s\", key: %s\n",
          at_utils_log__get_mod(conf->log),
          name, val_def, keys);
    }
  }

  *var = tmp;

  return 0;
}


int at_utils_conf__get_filename(
    at_utils_conf_t *conf,
    const char *keys,
    char **var,
    const char *val_def,
    const char *name)
{
  char *fn;

  at_utils_conf__get_str(conf, keys, &fn, val_def, name);

  *var = at_utils__make_output_filename(conf->ssm, conf->data_dir, fn);

  return 0;
}


#endif
