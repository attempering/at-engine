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
#include "../at_utils_misc.h"



void at_utils_conf__init(
    at_utils_conf_t *conf,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_utils_logger_t *logger,
    at_bool_t verbose)
{
  conf->cfg = cfg;
  conf->ssm = ssm;
  conf->data_dir = data_dir;

  conf->logger = logger;

  conf->verbose = verbose;

  conf->ready = AT__TRUE;
}


void at_utils_conf__finish(at_utils_conf_t *conf)
{
  memset(conf, '\0', sizeof(*conf));
  conf->ready = AT__FALSE;
}


void at_utils_conf__init_ez(
    at_utils_conf_t *conf,
    const char *cfg_fn,
    const char *data_dir,
    at_bool_t verbose)
{
  conf->ssm = zcom_ssm__open();

  conf->data_dir = data_dir ? zcom_ssm__dup(conf->ssm, data_dir) : NULL;

  if (cfg_fn != NULL) {
    conf->cfg = zcom_cfg__open(cfg_fn, conf->ssm, ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__ALLOW_DASHES);
  } else {
    conf->cfg = NULL;
  }

  at_utils__new(conf->logger, at_utils_logger_t);

  at_utils_logger__cfg_init(conf->logger, conf->cfg, conf->ssm, data_dir, verbose);

  conf->verbose = verbose;

  conf->ready = AT__TRUE;
}


void at_utils_conf__finish_ez(
    at_utils_conf_t *conf)
{
  at_utils_logger__finish(conf->logger);
  free(conf->logger);

  if (conf->cfg) {
    zcom_cfg__close(conf->cfg);
  }

  zcom_ssm__close(conf->ssm);

  at_utils_conf__finish(conf);
}


void at_utils_conf__push_mod(at_utils_conf_t *conf, const char *mod)
{
  at_utils_logger__push_mod(conf->logger, mod);
}


const char *at_utils_conf__pop_mod(at_utils_conf_t *conf)
{
  return at_utils_logger__pop_mod(conf->logger);
}


const char *at_utils_conf__get_mod(at_utils_conf_t *conf)
{
  return at_utils_logger__get_mod(conf->logger);
}


static void at_utils_conf__get_key_and_name(const char *keys, const char **key, const char **name)
{
  const char *p = strrchr(keys, ',');

  if (p != NULL) {
    *key = p + 1;
  } else {
    *key = keys;
  }

  if (*name == NULL) {
    *name = *key;
  }
}


int at_utils_conf__get_int(
    at_utils_conf_t *conf,
    const char *keys,
    int *var,
    int val_def,
    const char *name)
{
  const char *key;
  int tmp;

  at_utils_conf__get_key_and_name(keys, &key, &name);

  tmp = val_def;

  if (zcom_cfg__get(conf->cfg, &tmp, keys, "%d") != 0) {
    at_utils_logger__push_echo_state(conf->logger, conf->verbose);
    at_utils_logger__info(conf->logger, "assuming default %s = %d, key: %s\n",
        name, val_def, key);
    at_utils_logger__pop_echo_state(conf->logger);
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
  const char *key;
  double tmp;

  at_utils_conf__get_key_and_name(keys, &key, &name);

  tmp = val_def;

  if (zcom_cfg__get(conf->cfg, &tmp, keys, "%lf") != 0) {
    at_utils_logger__push_echo_state(conf->logger, conf->verbose);
    at_utils_logger__info(conf->logger, "assuming default %s = %g, key: %s\n",
        name, val_def, key);
    at_utils_logger__pop_echo_state(conf->logger);
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
  const char *key;
  char *tmp = zcom_ssm__dup(conf->ssm, val_def);

  at_utils_conf__get_key_and_name(keys, &key, &name);

  if (zcom_cfg__get(conf->cfg, &tmp, keys, "%s") != 0) {
    at_utils_logger__push_echo_state(conf->logger, conf->verbose);
    at_utils_logger__info(conf->logger, "assuming default %s = \"%s\", key: %s\n",
        name, val_def, key);
    at_utils_logger__pop_echo_state(conf->logger);
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
  char *fn = NULL;

  at_utils_conf__get_str(conf, keys, &fn, val_def, name);

  *var = at_utils__make_output_filename(conf->ssm, conf->data_dir, fn);

  return 0;
}


int at_utils_conf__get_bool(
    at_utils_conf_t *conf,
    const char *keys,
    at_bool_t *var,
    at_bool_t val_def,
    const char *name)
{
  const char *str_val_def = (val_def ? "true" : "false");
  const char *key;
  char *str_val = zcom_ssm__dup(conf->ssm, str_val_def);

  at_utils_conf__get_key_and_name(keys, &key, &name);

  if (zcom_cfg__get(conf->cfg, &str_val, keys, "%s") != 0) {
    at_utils_logger__push_echo_state(conf->logger, conf->verbose);
    at_utils_logger__info(conf->logger, "assuming default %s = %s, key: %s\n",
        name, str_val_def, key);
    at_utils_logger__pop_echo_state(conf->logger);
  }

  zcom_utils__str_to_lower(str_val);

  if (val_def) {

    if (strcmp(str_val, "0") == 0
     || strcmp(str_val, "false") == 0
     || strcmp(str_val, "no") == 0
     || strcmp(str_val, "off") == 0) {
      *var = AT__FALSE;
    } else {
      *var = AT__TRUE;
    }

  } else {

    if (strcmp(str_val, "1") == 0
     || strcmp(str_val, "true") == 0
     || strcmp(str_val, "yes") == 0
     || strcmp(str_val, "on") == 0) {
      *var = AT__TRUE;
    } else {
      *var = AT__FALSE;
    }

  }

  return 0;
}



#endif
