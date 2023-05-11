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

#ifndef AT_UTILS_TRACE_H__
#define AT_UTILS_TRACE_H__

#include "at_utils_trace__def.h"
#include "../../params/at_params__def.h"


void at_utils_trace__cfg_init(at_utils_trace_t *trace,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose);

zcom_log_t *at_utils_trace__open_file(at_utils_trace_t *trace);

void at_utils_trace__close_file(at_utils_trace_t *trace);

void at_utils_trace__finish(at_utils_trace_t *trace);

void at_utils_trace__manifest(at_utils_trace_t *trace, at_utils_manifest_t *manifest);

at_bool_t at_utils_trace__decide_do_trace(at_utils_trace_t *trace,
    const at_params_step_t *step_params);


#endif