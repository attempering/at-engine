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

/* binary IO routines for at_mb_t */
#ifndef AT_MB_IO_BINARY_H__
#define AT_MB_IO_BINARY_H__


#include "v2/at_mb_io_binary_v2.h"
#include "v3/at_mb_io_binary_v3.h"


#define AT_MB__FILE_BINARY_VERSION_LATEST 3

int at_mb__read_binary(
    at_mb_t *mb,
    const char *fn,
    int *version);


int at_mb__write_binary_versioned(
    at_mb_t *mb,
    const char *fn,
    int version);


__inline int at_mb__write_binary(
    at_mb_t *mb,
    const char *fn) {
  return at_mb__write_binary_versioned(mb, fn, AT_MB__FILE_BINARY_VERSION_LATEST);
}



#endif
