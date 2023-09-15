/* 
 * Copyright (C) 2006-2023  AT-Engine Developers
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

#ifndef ZCOM__LOGGER__SRC_H__
#define ZCOM__LOGGER__SRC_H__


#include "logger.h"


zcom_logger_t *zcom_logger__open(const char *fn, unsigned flags)
{
  zcom_logger_t *logger;

  if ((logger = (zcom_logger_t *) calloc(1, sizeof(*logger))) == NULL) {
    fprintf(stderr, "Fatal@zcom.logger: no memory for a new zcom_logger_t object\n");
    exit(1);
  }

  if (fn == NULL) {
    fn = "log.dat";
  }

  logger->fn = fn;
  logger->flags = flags;

  return logger;
}



int zcom_logger__vprintf(zcom_logger_t *logger, const char *fmt, va_list args)
{
  if (logger == NULL) {
    return 1;
  }

  if (logger->fp == NULL) {
    const char *aw = (logger->flags & ZCOM_LOGGER__APPEND) ? "a" : "w";
    if ((logger->fp = fopen(logger->fn, aw)) == NULL) {
      fprintf(stderr, "failed to open %s\n", logger->fn);
      return 1;
    }
  }

  if ((logger->flags & ZCOM_LOGGER__NO_OUTPUT_FILE) == 0) {
    vfprintf(logger->fp, fmt, args);
  }

  if (logger->flags & ZCOM_LOGGER__OUTPUT_SCREEN) {
    vprintf(fmt, args);
  }

  if (logger->flags & ZCOM_LOGGER__FLUSH_AFTERWARD) {
    zcom_logger__flush(logger);
  }

  return 0;
}



int zcom_logger__printf(zcom_logger_t *logger, const char *fmt, ...)
{
  va_list args;

  if (logger == NULL) {
    return 1;
  }

  if (logger->fp == NULL) {
    const char *aw = (logger->flags & ZCOM_LOGGER__APPEND) ? "a" : "w";
    if ((logger->fp = fopen(logger->fn, aw)) == NULL) {
      fprintf(stderr, "failed to open %s\n", logger->fn);
      return 1;
    }
  }

  if ((logger->flags & ZCOM_LOGGER__NO_OUTPUT_FILE) == 0) {
    va_start(args, fmt);
    vfprintf(logger->fp, fmt, args);
    va_end(args);
  }

  if (logger->flags & ZCOM_LOGGER__OUTPUT_SCREEN) {
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }

  if (logger->flags & ZCOM_LOGGER__FLUSH_AFTERWARD) {
    zcom_logger__flush(logger);
  }

  return 0;
}



void zcom_logger__close(zcom_logger_t *logger)
{
  if (logger == NULL) {
    return;
  }

  if (logger->fp != NULL) {
    fclose(logger->fp);
    logger->fp = NULL;
  }

  free(logger);
}



int zcom_logger__flush(zcom_logger_t *logger)
{
  if (logger->fp == NULL) {
    return 1;
  }

  fflush(logger->fp);

  return 0;
}



/* close & reopen logger file to make sure that stuff is written to disk */
int zcom_logger__hard_flush(zcom_logger_t *logger)
{
  if (logger->fp == NULL || logger->fn == NULL) return 1;

  fclose(logger->fp);

  if ((logger->fp = fopen(logger->fn, "a")) == NULL) {
    fprintf(stderr, "failed to open %s\n", logger->fn);
    return 1;
  }

  return 0;
}



#endif
