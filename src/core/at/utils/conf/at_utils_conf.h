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

#ifndef AT_UTILS_CONF_H__
#define AT_UTILS_CONF_H__

#include "at_utils_conf__def.h"

#include "../../../zcom/zcom.h"



void at_utils_conf__init(
    at_utils_conf_t *conf,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_utils_log_t *log,
    at_bool_t verbose);

void at_utils_conf__finish(at_utils_conf_t *conf);

/* easy initialization for testing programs */
void at_utils_conf__init_ez(
    at_utils_conf_t *conf,
    const char *cfg_fn,
    const char *data_dir,
    at_bool_t verbose);

/* cleaning up for __init_ez() */
void at_utils_conf__finish_ez(
    at_utils_conf_t *conf);

void at_utils_conf__push_mod(at_utils_conf_t *conf, const char *mod);

void at_utils_conf__pop_mod(at_utils_conf_t *conf);

int at_utils_conf__get_int(
    at_utils_conf_t *conf,
    const char *keys,
    int *var,
    int val_def,
    const char *name);

int at_utils_conf__get_double(
    at_utils_conf_t *conf,
    const char *keys,
    double *var,
    double val_def,
    const char *name);

int at_utils_conf__get_str(
    at_utils_conf_t *conf,
    const char *keys,
    char **var,
    const char *val_def,
    const char *name);

int at_utils_conf__get_filename(
    at_utils_conf_t *conf,
    const char *keys,
    char **var,
    const char *val_def,
    const char *name);

int at_utils_conf__get_bool(
    at_utils_conf_t *conf,
    const char *keys,
    at_bool_t *var,
    at_bool_t val_def,
    const char *name);

#endif
