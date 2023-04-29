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

#ifndef AT_LANGEVIN_ZEROFILLER_H__
#define AT_LANGEVIN_ZEROFILLER_H__


#include "../../mb/at_mb__def.h"


int at_langevin_zerofiller__init(at_langevin_zerofiller_t *zf, at_mb_t *mb);

void at_langevin_zerofiller__finish(at_langevin_zerofiller_t *zf);

double *at_langevin_zerofiller__fill_range_custom(at_langevin_zerofiller_t *zf,
    int ib_begin, int ib_end,
    bin_value_get_func_t bin_value_get_func);

double *at_langevin_zerofiller__fill_range_with_proper_sums(at_langevin_zerofiller_t *zf,
    int ib_begin, int ib_end);


#endif
