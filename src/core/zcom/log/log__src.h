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

#ifndef ZCOM__LOG__SRC_H__
#define ZCOM__LOG__SRC_H__


#include "log.h"


ZCOM__INLINE zcom_log_t *zcom_log__open(const char *fn, unsigned flags)
{
  zcom_log_t *log;

  if ((log = (zcom_log_t *) calloc(1, sizeof(*log))) == NULL) {
    fprintf(stderr, "Fatal: no memory for a new zcom_log_t object\n");
    exit(1);
  }

  if (fn == NULL) {
    fn = "LOG";
  }

  log->fname = fn;
  log->flags = flags;

  return log;
}



ZCOM__INLINE int zcom_log__vprintf(zcom_log_t *log, const char *fmt, va_list args)
{
  if (log == NULL) {
    return 1;
  }

  if (log->fp == NULL) {
    const char *aw = (log->flags & ZCOM_LOG__APPEND) ? "a" : "w";
    if ((log->fp = fopen(log->fname, aw)) == NULL) {
      fprintf(stderr, "failed to open %s\n", log->fname);
      return 1;
    }
  }

  if ((log->flags & ZCOM_LOG__NO_OUTPUT_FILE) == 0) {
    vfprintf(log->fp, fmt, args);
  }

  if (log->flags & ZCOM_LOG__OUTPUT_SCREEN) {
    vprintf(fmt, args);
  }

  if (log->flags & ZCOM_LOG__FLUSH_AFTER) {
    fflush(log->fp);
  }

  return 0;
}



ZCOM__INLINE int zcom_log__printf(zcom_log_t *log, const char *fmt, ...)
{
  va_list args;

  if (log == NULL) {
    return 1;
  }

  if (log->fp == NULL) {
    const char *aw = (log->flags & ZCOM_LOG__APPEND) ? "a" : "w";
    if ((log->fp = fopen(log->fname, aw)) == NULL) {
      fprintf(stderr, "failed to open %s\n", log->fname);
      return 1;
    }
  }

  if ((log->flags & ZCOM_LOG__NO_OUTPUT_FILE) == 0) {
    va_start(args, fmt);
    vfprintf(log->fp, fmt, args);
    va_end(args);
  }

  if (log->flags & ZCOM_LOG__OUTPUT_SCREEN) {
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }

  if (log->flags & ZCOM_LOG__FLUSH_AFTER) {
    fflush(log->fp);
  }

  return 0;
}



ZCOM__INLINE void zcom_log__close(zcom_log_t *log)
{
  if (log == NULL) {
    return;
  }

  if (log->fp != NULL) {
    fclose(log->fp);
    log->fp = NULL;
  }

  free(log);
}



/* close & reopen log file to make sure that stuff is written to disk */
ZCOM__INLINE int zcom_log__hard_flush(zcom_log_t *log)
{
  if (log->fp == NULL || log->fname == NULL) return 1;

  fclose(log->fp);

  if ((log->fp = fopen(log->fname, "a")) == NULL) {
    fprintf(stderr, "failed to open %s\n", log->fname);
    return 1;
  }

  return 0;
}



#endif
