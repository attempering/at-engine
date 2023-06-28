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

#ifndef AT_UTILS_IO__DEF_H__
#define AT_UTILS_IO__DEF_H__

#include <stdio.h>

typedef struct at_utils_io_t_
{
  const char *module;
  const char *fn;
  FILE *fp;
  int endn;
  int version;
} at_utils_io_t;

#define AT_UTILS_IO__VERBOSE        0x0001
#define AT_UTILS_IO__NONNEGATIVE    0x0100
#define AT_UTILS_IO__POSITIVE       0x0200
#define AT_UTILS_IO__ARRAY_COUNT    0x1000

#endif
