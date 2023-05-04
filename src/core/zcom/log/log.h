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

#ifndef ZCOM__LOG_H__
#define ZCOM__LOG_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../def/def.h"

typedef struct zcom_log_t_ {
  FILE *fp;
  const char *fname;
  unsigned flags;
} zcom_log_t;

#define ZCOM_LOG__OUTPUT_SCREEN   0x01
#define ZCOM_LOG__FLUSH_AFTER     0x02
#define ZCOM_LOG__NO_OUTPUT_FILE  0x10
#define ZCOM_LOG__APPEND          0x80

ZCOM__INLINE zcom_log_t *zcom_log__open(const char *fn);

ZCOM__INLINE int zcom_log__printf(zcom_log_t *log, char *fmt, ...);

ZCOM__INLINE void zcom_log__close(zcom_log_t *log);


/* close & reopen log file to make sure that stuff is written to disk */
ZCOM__INLINE int zcom_log__hard_flush(zcom_log_t *log);


#endif