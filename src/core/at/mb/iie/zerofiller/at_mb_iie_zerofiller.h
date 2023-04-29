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

#ifndef AT_MB_IIE_ZEROFILLER__H__
#define AT_MB_IIE_ZEROFILLER__H__


#include "at_mb_iie_zerofiller__def.h"


void at_mb_iie_zerofiller__init(at_mb_iie_zerofiller_t *zf, at_mb_t *mb);

void at_mb_iie_zerofiller__finish(at_mb_iie_zerofiller_t *zf);

/**
 * Given zf->vals[] and zf->has_vals[],
 * fill the values of empty bins in the range [ib_begin, ib_end].
 * Empty bins are marked by zf->has_vals[].
 * The filled values are written into zf->vals[].
 *
 * The algorithm we used here is a two-pass left/right scan algorithm.
 * 
 * return the number unfilled bins.
 * 
 * The return value is 0, if there is a single data point
 * in the specified range, or the number of bins in the range
 * on an empty range.
 */
int at_mb_iie_zerofiller__fill_missing(at_mb_iie_zerofiller_t *zf, int ib_begin, int ib_end);

#endif
