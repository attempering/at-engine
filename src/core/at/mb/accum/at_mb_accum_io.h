/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

#ifndef AT_MB_ACCUM_IO_H__
#define AT_MB_ACCUM_IO_H__


#include "at_mb_accum__def.h"
#include "../at_mb__def.h"
#include "../win/at_mb_win.h"
#include "../shk/at_mb_shk.h"
#include <stdio.h>

void at_mb_accum__manifest(const at_mb_accum_t *accum, FILE *fp, int arrmax);

int at_mb_accum__read_binary(at_mb_accum_t *accum, FILE *fp, int endn);

int at_mb_accum__write_binary(at_mb_accum_t *accum, FILE *fp);


#endif