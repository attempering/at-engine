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

#ifndef AT_UTILS_IO_TEXT_READ_H__
#define AT_UTILS_IO_TEXT_READ_H__

#include "../at_utils_io__def.h"

#include <stdio.h>

int at_utils_io_text__init_read(
    at_utils_io_t *io,
    const char *module,
    const char *fn,
    FILE *fp);

int at_utils_io_text__init_read_ex(
    at_utils_io_t *io,
    const char *module,
    const char *fn);

void at_utils_io_text__finish_read_ex(at_utils_io_t *io);

int at_utils_io_text__read_int(
    at_utils_io_t *io,
    int *val,
    const char *name,
    unsigned long flags);

int at_utils_io_text__read_and_compare_int(
    at_utils_io_t *io,
    int *val,
    const char *name,
    int val_ref);

int at_utils_io_text__read_double(
    at_utils_io_t *io,
    double *val,
    const char *name,
    unsigned long flags);

int at_utils_io_text__read_double_array(
    at_utils_io_t *io,
    int n,
    double *arr,
    const char *name,
    unsigned long flags);

int at_utils_io_text__read_and_compare_double(
    at_utils_io_t *io,
    double *val,
    const char *name,
    double val_ref,
    double tol);

#endif
