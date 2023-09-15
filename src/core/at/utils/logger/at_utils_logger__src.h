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

#ifndef AT_UTILS_LOGGER__SRC_H__
#define AT_UTILS_LOGGER__SRC_H__

#include "at_utils_logger.h"

#include "../at_utils_misc.h"
#include "../boolstack/at_utils_boolstack.h"
#include "../modstack/at_utils_modstack.h"
#include "../manifest/at_utils_manifest.h"



static void at_utils_logger__init_filename(
    at_utils_logger_t *logger,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  char *fn;

  fn = zcom_ssm__dup(ssm, "log.dat");
  if (zcom_cfg__get(cfg, &fn, "logger-file", "%s") != 0) {
    if (verbose) {
      fprintf(stderr, "Info@at.utils.logger: assuming default at.utils.logger.file = \"%s\", key: logger-file\n",
          fn);
    }
  }

  logger->file = at_utils__make_output_filename(ssm, data_dir, fn);
}



void at_utils_logger__cfg_init(
    at_utils_logger_t *logger,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  at_utils_logger__init_filename(logger, cfg, ssm, data_dir, verbose);

  logger->fp = NULL;

  logger->is_delegate = AT__FALSE;

  at_utils_modstack__init(logger->mods);

  at_utils_boolstack__init(logger->echo, AT__TRUE);

  logger->ready = AT__TRUE;
}



/* Note: delegate logger does not require __finish(); */
void at_utils_logger__init_delegate(
    at_utils_logger_t *logger_delegate,
    at_utils_logger_t *logger_src,
    const char *mod)
{
  logger_delegate->file = logger_src->file;

  logger_delegate->fp = logger_src->fp;

  logger_delegate->is_delegate = AT__TRUE;

  at_utils_modstack__init(logger_delegate->mods);
  at_utils_modstack__push(logger_delegate->mods, mod);

  at_utils_boolstack__init(logger_delegate->echo, AT__TRUE);

  logger_delegate->ready = AT__TRUE;
}



void at_utils_logger__finish(at_utils_logger_t *logger)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return;
  }

  if (!logger->is_delegate) {
    at_utils_logger__close_file(logger);
  }

  at_utils_boolstack__finish(logger->echo);

  at_utils_modstack__finish(logger->mods);
}



void at_utils_logger__manifest(at_utils_logger_t *logger, at_utils_manifest_t *manifest)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return;
  }

  at_utils_manifest__push_mod(manifest, "at.utils.logger");

  at_utils_manifest__print_str(manifest, logger->file, "file", "logger-file");

  at_utils_manifest__pop_mod(manifest);
}



void at_utils_logger__open_file(at_utils_logger_t *logger, at_bool_t is_continuation)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return;
  }

  logger->fp = fopen(logger->file,
      (is_continuation ? "a" : "w"));

  if (logger->fp == NULL) {
    fprintf(stderr, "Error@at.utils.logger: failed to open logger file [%s]\n", logger->file);
  }
}


void at_utils_logger__close_file(at_utils_logger_t *logger)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return;
  }

  if (logger->fp) {
    fclose(logger->fp);
    logger->fp = NULL;
  }
}


void at_utils_logger__flush(at_utils_logger_t *logger, at_bool_t hard)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return;
  }

  if (logger->fp) {
    if (hard) {
      // close and reopen the file
      fclose(logger->fp);
      logger->fp = fopen(logger->file, "a");
    } else {
      fflush(logger->fp);
    }
  }
}


void at_utils_logger__push_mod(at_utils_logger_t *logger, const char *mod)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return;
  }

  at_utils_modstack__push(logger->mods, mod);
}

const char *at_utils_logger__pop_mod(at_utils_logger_t *logger)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return NULL;
  }

  return at_utils_modstack__pop(logger->mods);
}

const char *at_utils_logger__get_mod(const at_utils_logger_t *logger)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return NULL;
  }

  return at_utils_modstack__get(logger->mods);
}





void at_utils_logger__push_echo_state(at_utils_logger_t *logger, at_bool_t echo)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return;
  }

  at_utils_boolstack__push(logger->echo, echo);
}

at_bool_t at_utils_logger__pop_echo_state(at_utils_logger_t *logger)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return AT__FALSE;
  }

  return at_utils_boolstack__pop(logger->echo);
}

at_bool_t at_utils_logger__get_echo_state(const at_utils_logger_t *logger)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return AT__FALSE;
  }

  return at_utils_boolstack__get(logger->echo);
}





static int at_utils_logger__vprintf_level_0(
    const char *module,
    const char *type,
    FILE *fp,
    at_bool_t fp_is_terminal,
    const char *fmt,
    va_list args)
{
  if (fp == NULL) {
    return -1;
  }

  if (type != NULL) {
    if (fp_is_terminal) {
      fprintf(fp, "\r");
    }
    if (module != NULL) {
      fprintf(fp, "%s@%s: ", type, module);
    } else {
      fprintf(fp, "%s: ", type);
    }
  }

  vfprintf(fp, fmt, args);

  return 0;
}


static void at_utils_logger__vprintf_file(
    at_utils_logger_t *logger,
    const char *module,
    const char *type,
    const char *fmt, va_list args)
{
  if (logger == NULL || logger->ready != AT__TRUE) {
    return;
  }

  if (logger->fp == NULL && !logger->is_delegate) {
    if (logger->file == NULL) {
      at_utils_logger__vprintf_level_0(module, type, stderr, AT__TRUE, fmt, args);
      return;
      //fprintf(stderr, "Error@at.utils.logger: trying to print, module %s, type %s, fmt %s\n",
      //    module, type, fmt);
    }
    at_utils_logger__open_file(logger, AT__FALSE);
  }

  at_utils_logger__vprintf_level_0(module, type, logger->fp, AT__FALSE, fmt, args);
}


static void at_utils_logger__vprintf_stderr(
    const char *module,
    const char *type,
    const char *fmt, va_list args)
{
  at_utils_logger__vprintf_level_0(module, type, stderr, AT__TRUE, fmt, args);
}



/* template for at_utils_logger__info()/warning()/error(), etc */
#define AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, tag, fmt) \
{ \
  const char *module; \
  va_list args; \
  \
  if (logger == NULL || logger->ready != AT__TRUE) { \
    return; \
  } \
  module = at_utils_logger__get_mod(logger); \
  \
  va_start(args, fmt); \
  at_utils_logger__vprintf_file(logger, module, tag, fmt, args); \
  va_end(args); \
  \
  if (at_utils_boolstack__get(logger->echo)) { \
    va_start(args, fmt); \
    at_utils_logger__vprintf_stderr(module, tag, fmt, args); \
    va_end(args); \
  } \
}



void at_utils_logger__printf(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, NULL, fmt);
}


void at_utils_logger__printf_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  at_utils_logger__push_echo_state(logger, AT__FALSE);
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, NULL, fmt);
  at_utils_logger__pop_echo_state(logger);
}


void at_utils_logger__info(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, "Info", fmt);
}


void at_utils_logger__info_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  at_utils_logger__push_echo_state(logger, AT__FALSE);
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, "Info", fmt);
  at_utils_logger__pop_echo_state(logger);
}


void at_utils_logger__warning(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, "Warning", fmt);
}


void at_utils_logger__warning_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  at_utils_logger__push_echo_state(logger, AT__FALSE);
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, "Warning", fmt);
  at_utils_logger__pop_echo_state(logger);
}


void at_utils_logger__error(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, "Error", fmt);
}


void at_utils_logger__error_no_echo(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  at_utils_logger__push_echo_state(logger, AT__FALSE);
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, "Error", fmt);
  at_utils_logger__pop_echo_state(logger);
}



void at_utils_logger__fatal(
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, "Fatal", fmt);

  exit(1);
}



void at_utils_logger__exit_if(
    at_bool_t cond,
    at_utils_logger_t *logger,
    const char *fmt, ...)
{
  if (!cond) {
    return;
  }

  AT_UTILS_LOGGER__TAGGED_PRINTF__(logger, "Fatal", fmt);

  exit(1);
}



#endif
