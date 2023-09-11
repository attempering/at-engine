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

#ifndef ZCOM__XDOUBLE_H__
#define ZCOM__XDOUBLE_H__

#include "xdouble__def.h"

zcom_xdouble_t zcom_xdouble__get_zero(void);

int zcom_xdouble__is_zero(zcom_xdouble_t x);

zcom_xdouble_t zcom_xdouble__get_inf(void);

int zcom_xdouble__is_inf(zcom_xdouble_t x);

zcom_xdouble_t zcom_xdouble__from_double(double y);

double zcom_xdouble__to_double(zcom_xdouble_t x);

zcom_xdouble_t zcom_xdouble__add(zcom_xdouble_t x, zcom_xdouble_t y);

zcom_xdouble_t zcom_xdouble__neg(zcom_xdouble_t x);

zcom_xdouble_t zcom_xdouble__sub(zcom_xdouble_t x, zcom_xdouble_t y);

zcom_xdouble_t zcom_xdouble__mul(zcom_xdouble_t x, zcom_xdouble_t y);

zcom_xdouble_t zcom_xdouble__inv(zcom_xdouble_t x);

zcom_xdouble_t zcom_xdouble__div(zcom_xdouble_t x, zcom_xdouble_t y);

zcom_xdouble_t zcom_xdouble__exp(double exponent);

int zcom_xdouble__compare(zcom_xdouble_t x, zcom_xdouble_t y);

int zcom_xdouble__compare_double(zcom_xdouble_t x, double y);

zcom_xdouble_t zcom_xdouble__clamp(zcom_xdouble_t x, zcom_xdouble_t x_min, zcom_xdouble_t x_max);

zcom_xdouble_t zcom_xdouble__clamp_double(zcom_xdouble_t x, double x_min, double x_max);

#endif
