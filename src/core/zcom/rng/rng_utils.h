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

#ifndef ZCOM__RNG_UTILS_H__
#define ZCOM__RNG_UTILS_H__


inline static double zcom_rng__uint32_to_01__(uint32_t x) {
  return x * (1.0/4294967296.0);
}

inline static double zcom_rng__uint64_to_01__(uint64_t x) {
  return x * (1.0/18446744073709551616.0);
}


/* Gaussian distribution with zero mean and unit variance
 * using the ratio method */
#define ZCOM_RNG__BUILD_RAND_GAUSS__(gen, rand_01) \
{ \
  double x, y, u, v, q; \
  do { \
    u = 1 - rand_01(gen); \
    v = 1.7156*(rand_01(gen) - .5);  /* >= 2*sqrt(2/e) */ \
    x = u - 0.449871; \
    y = fabs(v) + 0.386595; \
    q = x*x  + y*(0.196*y - 0.25472*x); \
    if (q < 0.27597) break; \
  } while (q > 0.27846 || v*v > -4*u*u*log(u)); \
  return v/u; \
}

#endif
