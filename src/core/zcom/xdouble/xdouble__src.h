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

#ifndef ZCOM__XDOUBLE__SRC_H__
#define ZCOM__XDOUBLE__SRC_H__

#include "xdouble.h"


zcom_xdouble_t zcom_xdouble__get_zero(void)
{
  zcom_xdouble_t x = {
    0.5,
    ZCOM__XDOUBLE__EXP_MIN
  };

  return x;
}


int zcom_xdouble__is_zero(zcom_xdouble_t x)
{
  return x.exp < ZCOM__XDOUBLE__EXP_MIN;
}


zcom_xdouble_t zcom_xdouble__get_inf(void)
{
  zcom_xdouble_t x = {
    0.5,
    ZCOM__XDOUBLE__EXP_MAX
  };

  return x;
}


int zcom_xdouble__is_inf(zcom_xdouble_t x)
{
  return x.exp > ZCOM__XDOUBLE__EXP_MAX;
}


zcom_xdouble_t zcom_xdouble__from_double(double y)
{
  zcom_xdouble_t x;

  if (y == 0.0) {
    return zcom_xdouble__get_zero();
  } else {
    x.man = frexp(y, &x.exp);
  }

  return x;
}


inline static double zcom_xdouble__pow2i_(int exponent) {
  return pow(2.0, exponent);
} 


double zcom_xdouble__to_double(zcom_xdouble_t x)
{
  return x.man * zcom_xdouble__pow2i_(x.exp);
}


zcom_xdouble_t zcom_xdouble__add(zcom_xdouble_t x, zcom_xdouble_t y)
{
  zcom_xdouble_t z;

  /* comparison of magnitude is meaningless if one of the */
  if (zcom_xdouble__is_zero(x)) {
    return y;
  } else if (zcom_xdouble__is_zero(y)) {
    return x;
  }

  if (x.exp > y.exp) {
    z.man = x.man + y.man * zcom_xdouble__pow2i_(y.exp - x.exp);
    z.exp = x.exp;
  } else {
    z.man = y.man + x.man * zcom_xdouble__pow2i_(x.exp - y.exp);
    z.exp = y.exp;
  }

  /* normalize the result */
  if (z.man > 0) {

    if (z.man >= 1) {
      z.man *= 0.5;
      z.exp++;
    } else {
      while (z.man < 0.5) {
        z.man *= 2;
        z.exp--;
      }
    }

  } else if (z.man < 0) {

    if (z.man <= -1.0) {
      z.man *= 0.5;
      z.exp++;
    } else {
      while (z.man > -0.5) {
        z.man *= 2;
        z.exp--;
      }
    }

  } else { /* z.man == 0.0 */

    z = zcom_xdouble__get_zero();

  }

  return z;
}


zcom_xdouble_t zcom_xdouble__neg(zcom_xdouble_t x)
{
  x.man = -x.man;
  return x;
}


zcom_xdouble_t zcom_xdouble__sub(zcom_xdouble_t x, zcom_xdouble_t y)
{
  return zcom_xdouble__add(x, zcom_xdouble__neg(y));
}


zcom_xdouble_t zcom_xdouble__mul(zcom_xdouble_t x, zcom_xdouble_t y)
{
  zcom_xdouble_t z;

  z.man = x.man * y.man;
  z.exp = x.exp + y.exp;

  /* normalize the result */
  if (z.man > 0) {

    while (z.man < 0.5) {
      z.man *= 2;
      z.exp--;
    }

  } else if (z.man < 0) {

    while (z.man > -0.5) {
      z.man *= 2;
      z.exp--;
    }

  } else { /* z.man == 0.0 */

    z = zcom_xdouble__get_zero();

  }

  return z;
}


zcom_xdouble_t zcom_xdouble__inv(zcom_xdouble_t x)
{
  x.man = 0.5/x.man;
  x.exp = 1 - x.exp;
  return x;
}


zcom_xdouble_t zcom_xdouble__div(zcom_xdouble_t x, zcom_xdouble_t y)
{
  return zcom_xdouble__mul(x, zcom_xdouble__inv(y));
}


/* return exp(exponent) = 2^[log2(e)*exponent] */
zcom_xdouble_t zcom_xdouble__exp(double exponent)
{
  double exp_base2;
  int exp_base2_i;
  zcom_xdouble_t x;
  static double LOG_2_E = 0.0;

  if (LOG_2_E == 0.0) {
    /* log2(e) = 1/ln(2) */
    LOG_2_E = 1.0/log(2.0);
  }

  exp_base2 = exponent * LOG_2_E;
  exp_base2_i = floor(exp_base2) + 1;

  x.man = pow(2.0, exp_base2 - exp_base2_i);
  x.exp = exp_base2_i;

  return x;
}



#endif
