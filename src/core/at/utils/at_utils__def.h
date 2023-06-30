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

#ifndef AT_UTILS__DEF_H__
#define AT_UTILS__DEF_H__

#include "modstack/at_utils_modstack__def.h"
#include "lockfile/at_utils_lockfile__def.h"
#include "manifest/at_utils_manifest__def.h"
#include "io/at_utils_io__def.h"
#include "log/at_utils_log__def.h"
#include "trace/at_utils_trace__def.h"

typedef struct at_utils_t_
{
  int ready;

  at_utils_lockfile_t lockfile[1];
  at_utils_manifest_t manifest[1];
  at_utils_log_t log[1]; /* human-readable log file */
  at_utils_trace_t trace[1]; /* plot-able linear log file */

  double thermostat_temp;

  // name of the data directory,
  // to be constructed from the program
  char *data_dir;

  zcom_ssm_t *ssm;
} at_utils_t;

#endif
