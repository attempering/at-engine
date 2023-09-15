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

#ifndef ZCOM__LOGGER_H__
#define ZCOM__LOGGER_H__


#include "logger__def.h"

zcom_logger_t *zcom_logger__open(const char *fn, unsigned flags);

int zcom_logger__vprintf(zcom_logger_t *logger, const char *fmt, va_list args);

int zcom_logger__printf(zcom_logger_t *logger, const char *fmt, ...);

void zcom_logger__close(zcom_logger_t *logger);

int zcom_logger__flush(zcom_logger_t *logger);

/* close & reopen logger file to make sure that stuff is written to disk */
int zcom_logger__hard_flush(zcom_logger_t *logger);


#endif
