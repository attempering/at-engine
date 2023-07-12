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

/* text IO routines for at_mb_t */
#ifndef AT_MB_IO_TEXT_H__
#define AT_MB_IO_TEXT_H__

#include "v3/at_mb_io_text_v3.h"

#define AT_MB__FILE_TEXT_VERSION_LATEST 3

int at_mb__read_text(
    at_mb_t *mb,
    const char *fn,
    int *pver);


int at_mb__write_text_versioned(
    at_mb_t *mb,
    const char *fn,
    int ver);


int at_mb__write_text(
    at_mb_t *mb,
    const char *fn);



#endif
