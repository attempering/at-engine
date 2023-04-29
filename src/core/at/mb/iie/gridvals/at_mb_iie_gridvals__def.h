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

#ifndef AT_MB_IIE_GRIDVALS__DEF_H__
#define AT_MB_IIE_GRIDVALS__DEF_H__

/* quality bits */
#define AT_MB_IIE_GRIDVALS__QUALITY_BIT_EHAT  0x00000002
#define AT_MB_IIE_GRIDVALS__QUALITY_BIT_CV    0x00000004
#define AT_MB_IIE_GRIDVALS__QUALITY_BIT_LNZ   0x00000008

typedef struct at_mb_iie_gridvals_item_t_ {
  double lnz;
  double e;
  double cv;
  unsigned quality_bits;
} at_mb_iie_gridvals_item_t;


typedef struct at_mb_iie_gridvals_t_ {
  int n;
  at_mb_iie_gridvals_item_t *items;
} at_mb_iie_gridvals_t;



#endif
