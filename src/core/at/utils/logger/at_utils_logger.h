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

#ifndef AT_UTILS_LOGGER_H__
#define AT_UTILS_LOGGER_H__

#include "at_utils_logger__def.h"
#include "../manifest/at_utils_manifest__def.h"
#include "../../params/at_params__def.h"


void at_utils_logger__cfg_init(
    at_utils_logger_t *logger,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose);

/* initialize a logger for a fixed module */
void at_utils_logger__init_delegate(
    at_utils_logger_t *logger_delegate,
    at_utils_logger_t *logger_src,
    const char *mod);

void at_utils_logger__open_file(at_utils_logger_t *logger, at_bool_t is_continuation);

void at_utils_logger__close_file(at_utils_logger_t *logger);

void at_utils_logger__finish(at_utils_logger_t *logger);

void at_utils_logger__flush(at_utils_logger_t *logger, at_bool_t hard);

void at_utils_logger__manifest(
    const at_utils_logger_t *logger,
    at_utils_manifest_t *manifest);



void at_utils_logger__push_mod(at_utils_logger_t *logger, const char *mod);

const char *at_utils_logger__pop_mod(at_utils_logger_t *logger);

const char *at_utils_logger__get_mod(const at_utils_logger_t *logger);



void at_utils_logger__push_echo_state(at_utils_logger_t *logger, at_bool_t echo);

at_bool_t at_utils_logger__pop_echo_state(at_utils_logger_t *logger);

at_bool_t at_utils_logger__get_echo_state(const at_utils_logger_t *logger);



void at_utils_logger__vprintf(
    at_utils_logger_t *logger,
    const char *type,
    const char *fmt, va_list args);

void at_utils_logger__printf(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__printf_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__debug(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__debug_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__info(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__info_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__warning(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__warning_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__error(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__error_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__fatal(
    at_utils_logger_t *logger,
    const char *fmt, ...);

void at_utils_logger__exit_if(
    at_bool_t cond,
    at_utils_logger_t *logger,
    const char *fmt, ...);


#endif
