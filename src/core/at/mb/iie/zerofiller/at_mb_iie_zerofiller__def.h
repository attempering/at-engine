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

#ifndef AT_MB_IIE_ZEROFILLER__DEF_H__
#define AT_MB_IIE_ZEROFILLER__DEF_H__

typedef struct at_mb_t_ at_mb_t;

typedef struct at_mb_iie_zerofiller_item_t_ {
  double w;
  double mean;
  double var;
} at_mb_iie_zerofiller_item_t;

typedef struct at_mb_iie_zerofiller_t_ {
  int n; /* same as distr->domain->n */

  /* In an earlier design, we kept a record of *raw_vals
   * in addition to the *vals. This is now abandoned.
   * After the simulation accumulates enough data points,
   * all data points are filled. In such a case,
   * the zerofiller should perform as few operations
   * as possible to achieve the best performance.
   * Copying values from raw_vals to vals would be
   * too time-consuming in a case. */
  at_mb_iie_zerofiller_item_t   *vals;     /* filled values */
  int      *has_vals;

  int      *ib_from_left;
  int      *ib_from_right;

  at_mb_t *mb;
} at_mb_iie_zerofiller_t;


#endif
