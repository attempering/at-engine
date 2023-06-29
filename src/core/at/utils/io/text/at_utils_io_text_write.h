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

#ifndef AT_UTILS_IO_TEXT_WRITE_H__
#define AT_UTILS_IO_TEXT_WRITE_H__

#include "../at_utils_io__def.h"

#include <stdio.h>

int at_utils_io_text__init_write(
    at_utils_io_t *io,
    const char *module,
    const char *fn,
    FILE *fp);

int at_utils_io_text__init_write_ex(
    at_utils_io_t *io,
    const char *module,
    const char *fn,
    int version);

void at_utils_io_text__finish_write_ex(at_utils_io_t *io);

int at_utils_io_text__write_int(
    at_utils_io_t *io,
    int val,
    const char *name);

int at_utils_io_text__write_int_array(
    at_utils_io_t *io,
    int n,
    int *arr,
    const char *name,
    unsigned long flags);

int at_utils_io_text__write_double(
    at_utils_io_t *io,
    double val,
    const char *name);

int at_utils_io_text__write_double_array(
    at_utils_io_t *io,
    int n,
    double *arr,
    const char *name,
    unsigned long flags);

#endif
