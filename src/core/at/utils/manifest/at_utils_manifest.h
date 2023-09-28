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

#ifndef AT_UTILS_MANIFEST_H__
#define AT_UTILS_MANIFEST_H__

#include "at_utils_manifest__def.h"
#include "../conf/at_utils_conf.h"

void at_utils_manifest__conf_init(
    at_utils_manifest_t *manifest,
    at_utils_conf_t *conf);

FILE *at_utils_manifest__open_file(at_utils_manifest_t *manifest);

FILE *at_utils_manifest__append_file(at_utils_manifest_t *manifest);

void at_utils_manifest__close_file(at_utils_manifest_t *manifest);

void at_utils_manifest__finish(at_utils_manifest_t *manifest);

void at_utils_manifest__push_mod(at_utils_manifest_t *manifest, const char *mod);

const char *at_utils_manifest__pop_mod(at_utils_manifest_t *manifest);

const char *at_utils_manifest__get_mod(at_utils_manifest_t *manifest);

void at_utils_manifest__manifest(at_utils_manifest_t *manifest);

void at_utils_manifest__print_int(at_utils_manifest_t *manifest,
    int value, const char *var, const char *cfg_key);

void at_utils_manifest__print_double(at_utils_manifest_t *manifest,
    double value, const char *var, const char *cfg_key);

void at_utils_manifest__print_str(at_utils_manifest_t *manifest,
    const char* value, const char *var, const char *cfg_key);

void at_utils_manifest__print_bool(at_utils_manifest_t *manifest,
    at_bool_t value, const char *var, const char *cfg_key);

void at_utils_manifest__print_int_arr(at_utils_manifest_t *manifest,
    const int *arr, int n, const char *name);

void at_utils_manifest__print_double_arr(at_utils_manifest_t *manifest,
    const double *arr, int n, const char *name);

#endif
