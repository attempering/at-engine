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

#ifndef AT_UTILS_LOG__SRC_H__
#define AT_UTILS_LOG__SRC_H__

#include "at_utils_log.h"

#include "../../../zcom/zcom.h"
#include "../at_utils_misc.h"
#include "../modstack/at_utils_modstack.h"
#include "../manifest/at_utils_manifest__def.h"
#include "../../../zcom/zcom.h"



void at_utils_log__cfg_init(
    at_utils_log_t *log,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  char *fn;

  fn = zcom_ssm__dup(ssm, "log.dat");
  if (zcom_cfg__get(cfg, &fn, "log-file", "%s") != 0) {
    if (verbose) fprintf(stderr, "Info@at.utils.log: assuming default at.utils.log.file = \"%s\", key: log-file\n",
        fn);
  }
  log->file = at_utils__make_output_filename(ssm, data_dir, fn);

  log->fp = NULL;

  at_utils_modstack__init(log->mods);

  log->ready = AT__TRUE;
}



void at_utils_log__finish(at_utils_log_t *log)
{
  if (log->ready) {
    at_utils_log__close_file(log);
    at_utils_modstack__finish(log->mods);
  }
}



void at_utils_log__manifest(at_utils_log_t *log, at_utils_manifest_t *manifest)
{
  fprintf(manifest->fp, "utils->log->file: char *, %s\n", log->file);
}




void at_utils_log__open_file(at_utils_log_t *log, at_bool_t is_continuation)
{
  log->fp = fopen(log->file,
      (is_continuation ? "a" : "w"));

  if (log->fp == NULL) {
    fprintf(stderr, "Error@at.utils.log: failed to open log file [%s]\n", log->file);
  }
}


void at_utils_log__close_file(at_utils_log_t *log)
{
  if (log->fp) {
    fclose(log->fp);
    log->fp = NULL;
  }
}


void at_utils_log__push_mod(at_utils_log_t *log, const char *mod)
{
  if (log == NULL) {
    return;
  }

  at_utils_modstack__push(log->mods, mod);
}

const char *at_utils_log__pop_mod(at_utils_log_t *log)
{
  if (log == NULL) {
    return NULL;
  }

  return at_utils_modstack__pop(log->mods);
}

const char *at_utils_log__get_mod(at_utils_log_t *log)
{
  if (log == NULL) {
    return NULL;
  }

  return at_utils_modstack__get(log->mods);
}




static int at_utils_log__vprintf_level_0(
    const char *module,
    const char *type,
    FILE *fp,
    const char *fmt, va_list args)
{
  if (fp == NULL) {
    return -1;
  }

  if (module != NULL) {
    fprintf(fp, "%s@%s: ", type, module);
  } else {
    fprintf(fp, "%s: ", type);
  }

  vfprintf(fp, fmt, args);

  return 0;
}


static void at_utils_log__vprintf_file(
    at_utils_log_t *log,
    const char *module,
    const char *type,
    const char *fmt, va_list args)
{
  if (log->fp == NULL) {
    at_utils_log__open_file(log, AT__FALSE);
  }

  at_utils_log__vprintf_level_0(module, type, log->fp, fmt, args);
}


static void at_utils_log__vprintf_stderr(
    const char *module,
    const char *type,
    const char *fmt, va_list args)
{
  at_utils_log__vprintf_level_0(module, type, stderr, fmt, args);
}


void at_utils_log__printf(
    at_utils_log_t *log,
    const char *type,
    const char *fmt, ...)
{
  const char *module;
  va_list args;

  if (log == NULL) { /* fall back to fprintf(stderr, fmt, ...) */
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
  }

  module = at_utils_log__get_mod(log);

  va_start(args, fmt);
  at_utils_log__vprintf_file(log, module, type, fmt, args);
  va_end(args);

  va_start(args, fmt);
  at_utils_log__vprintf_stderr(module, type, fmt, args);
  va_end(args);
}


void at_utils_log__info(
    at_utils_log_t *log,
    const char *fmt, ...)
{
  const char *module;
  va_list args;

  module = at_utils_log__get_mod(log);

  va_start(args, fmt);
  at_utils_log__vprintf_file(log, module, "Info", fmt, args);
  va_end(args);

  va_start(args, fmt);
  at_utils_log__vprintf_stderr(module, "Info", fmt, args);
  va_end(args);
}


void at_utils_log__warning(
    at_utils_log_t *log,
    const char *fmt, ...)
{
  const char *module;
  va_list args;

  module = at_utils_log__get_mod(log);

  va_start(args, fmt);
  at_utils_log__vprintf_file(log, module, "Warning", fmt, args);
  va_end(args);

  va_start(args, fmt);
  at_utils_log__vprintf_stderr(module, "Warning", fmt, args);
  va_end(args);
}


void at_utils_log__error(
    at_utils_log_t *log,
    const char *fmt, ...)
{
  const char *module;
  va_list args;

  module = at_utils_log__get_mod(log);

  va_start(args, fmt);
  at_utils_log__vprintf_file(log, module, "Error", fmt, args);
  va_end(args);

  va_start(args, fmt);
  at_utils_log__vprintf_stderr(module, "Error", fmt, args);
  va_end(args);
}


void at_utils_log__exit_if(
    at_bool_t cond,
    at_utils_log_t *log,
    const char *fmt, ...)
{
  const char *module;
  va_list args;

  if (!cond) return;

  module = at_utils_log__get_mod(log);

  va_start(args, fmt);
  at_utils_log__vprintf_file(log, module, "Error", fmt, args);
  va_end(args);

  va_start(args, fmt);
  at_utils_log__vprintf_stderr(module, "Error", fmt, args);
  va_end(args);

  exit(1);
}


#endif
