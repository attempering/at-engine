/* 
 * Copyright (C) 2006-2023  AT-Engine Developers
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

#ifndef ZCOM__XDOUBLE__DEF_H__
#define ZCOM__XDOUBLE__DEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <tgmath.h>
#include <limits.h>

#include "../def/def.h"

/* extended double with larger exponent
 *
 * Note: the precision is still limited by that of
 * a regular double.
 * So if the calculation involves heavy cancellation,
 * the solution proposed here will not work.
 * But if the difficulty is mainly due to
 * some large exponent, e.g., exp(-xxx),
 * then it may work fine.
 **/
typedef struct zcom_xdouble_t_ {
  double man; /* mantissa or significant, [0.5, 1) or (-1, -0.5] */
  int exp; /* additional exponent */
} zcom_xdouble_t;

#define ZCOM__XDOUBLE__EXP_MIN  ((INT_MIN)/16)
#define ZCOM__XDOUBLE__EXP_MAX  (1-ZCOM__XDOUBLE__EXP_MIN)

#endif
