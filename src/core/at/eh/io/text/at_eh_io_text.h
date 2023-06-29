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

#ifndef AT_EH_IO_TEXT_H__
#define AT_EH_IO_TEXT_H__

#include "../../at_eh__def.h"

/* at_eh__read_text: read at_mb_t/eh data as text */
int at_eh__read_text(at_eh_t *eh, const char *fn, int *version);

/* at_eh__write_text: write at_mb_t/eh data as text */
int at_eh__write_text(at_eh_t *eh, const char *fn, int version);

#endif
