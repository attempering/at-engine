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

#ifndef AT_MB_ACCUM_WINACCUM_ITEM_IO_TEXT_H__
#define AT_MB_ACCUM_WINACCUM_ITEM_IO_TEXT_H__


#include "../../at_mb_accum_winaccum__def.h"
#include "../../../../../utils/at_utils.h"

int at_mb_accum_winaccum_item__read_text(
    at_mb_accum_winaccum_item_t *item,
    at_utils_io_t *io);

int at_mb_accum_winaccum_item__write_text(
    at_mb_accum_winaccum_item_t *item,
    at_utils_io_t *io);

#endif
