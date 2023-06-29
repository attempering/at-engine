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

#ifndef AT_DRIVER_LANGEVIN_ZEROFILLER__SRC_H__
#define AT_DRIVER_LANGEVIN_ZEROFILLER__SRC_H__


/* implementation dependent headers */
#include "../../../mb/at_mb_basic.h"
#include "../../../mb/accum/at_mb_accum.h"

#include "at_driver_langevin_zerofiller.h"


int at_driver_langevin_zerofiller__init(
    at_driver_langevin_zerofiller_t *zf,
    int n)
{
  int i;

  zf->n = n;

  if ((zf->w = (double *) calloc((n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "Error@at.driver.langevin.zerofiller: no memory! var: zf->w, type: double\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    zf->w[i] = 0;
  }

  if ((zf->vals = (double *) calloc((n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "Error@at.driver.langevin.zerofiller: no memory! var: zf->vals, type: double\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    zf->vals[i] = 0;
  }

  if ((zf->has_vals = (int *) calloc((n + 1), sizeof(int))) == NULL) {
    fprintf(stderr, "Error@at.driver.langevin.zerofiller: no memory! var: zf->has_vals, type: int\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    zf->has_vals[i] = 0;
  }


  if ((zf->ib_from_left = (int *) calloc((n + 1), sizeof(int))) == NULL) {
    fprintf(stderr, "Error@at.driver.langevin.zerofiller: no memory! var: zf->ib_from_left, type: int\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    zf->ib_from_left[i] = 0;
  }


  if ((zf->ib_from_right = (int *) calloc((n + 1), sizeof(int))) == NULL) {
    fprintf(stderr, "Error@at.driver.langevin.zerofiller: no memory! var: zf->ib_from_right, type: int\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    zf->ib_from_right[i] = 0;
  }

  return 0;
}



void at_driver_langevin_zerofiller__finish(at_driver_langevin_zerofiller_t *zf)
{
  free(zf->w);
  free(zf->vals);
  free(zf->has_vals);
  free(zf->ib_from_left);
  free(zf->ib_from_right);
}



static double at_driver_langevin_zerofiller__get_raw_bin_value_from_proper_sum(
      at_mb_t *mb, int ib, int *has_val, double *w)
{
  at_mb_sm_t *sm = at_mb_accum__get_proper_sums(mb->accum, ib, ib);

  double count = sm->s;

  *w = count;

  if (count > AT_MB_ACCUM__MIN_SIZE) {

    *has_val = 1;

    return sm->se / count;

  } else {

    *has_val = 0;

    return 0.0;

  }
}



static void at_driver_langevin_zerofiller__fill_raw_values(
    at_driver_langevin_zerofiller_t *zf,
    int ib_begin, int ib_end,
    bin_value_get_func_t bin_value_get_func,
    at_mb_t *mb)
{
  int ib;

  for (ib = ib_begin; ib <= ib_end; ib++) {

    zf->vals[ib] = (*bin_value_get_func)(mb, ib,
        &zf->has_vals[ib],
        &zf->w[ib]);

  }

}


static int at_driver_langevin_zerofiller__fill_missing_values(
    at_driver_langevin_zerofiller_t *zf,
    int ib_begin, int ib_end)
{
  int ib;
  int n = zf->n;
  int dist_infinity = n * 10;
  int errors = 0;
  const int print_debug_info = 0;

  // left-to-right scan
  {
    int ib_from_left = -1;

    for (ib = ib_begin; ib <= ib_end; ib++) {

      if (zf->has_vals[ib]) {
        ib_from_left = ib;
      } else {
        zf->ib_from_left[ib] = ib_from_left;
      }

    }
  }


  // right-to-left scan
  {
    int ib_from_right = -1;

    for (ib = ib_end; ib >= ib_begin; ib--) {

      if (zf->has_vals[ib]) {
        ib_from_right = ib;
      } else {
        zf->ib_from_right[ib] = ib_from_right;
      }

    }
  }


  // selection

  for (ib = ib_begin; ib <= ib_end; ib++) {

    if (zf->has_vals[ib]) {
      continue;
    }

    int ib_from_left = zf->ib_from_left[ib];
    int dist_from_left = (ib_from_left >= 0 ? ib - ib_from_left : dist_infinity);

    int ib_from_right = zf->ib_from_right[ib];
    int dist_from_right = (ib_from_right >= 0 ? ib_from_right - ib : dist_infinity);

    if (dist_from_left < dist_from_right) {

      // use the left value
      zf->vals[ib] = zf->vals[ib_from_left];

    } else if (dist_from_left > dist_from_right) {

      // use the right value
      zf->vals[ib] = zf->vals[ib_from_right];

    } else {

      // equal distances
      if (ib_from_left >= 0 && ib_from_right >= 0) {

        double wl = zf->w[ib_from_left] + 1e-30;
        double wr = zf->w[ib_from_right] + 1e-30;
        zf->vals[ib] = (wl * zf->vals[ib_from_left] + wr * zf->vals[ib_from_right])
                     / (wl + wr);

      } else {
        zf->vals[ib] = 0;
        errors++;
      }

    }

    if (print_debug_info) {
      printf("ib %d, has %d, val %g, val_from_left %g,%d, val_from_right %g,%d\n",
          ib, zf->has_vals[ib], zf->vals[ib],
          zf->vals[ib_from_left], dist_from_left,
          zf->vals[ib_from_right], dist_from_right);
    }

  }

  // getchar();
  return errors;
}



double *at_driver_langevin_zerofiller__fill_range_custom(
    at_driver_langevin_zerofiller_t *zf,
    int ib_begin, int ib_end,
    bin_value_get_func_t bin_value_get_func,
    at_mb_t *mb)
{
  at_driver_langevin_zerofiller__fill_raw_values(
      zf, ib_begin, ib_end,
      bin_value_get_func, mb);

  at_driver_langevin_zerofiller__fill_missing_values(
      zf, ib_begin, ib_end);

  return zf->vals;
}



double *at_driver_langevin_zerofiller__fill_range_with_proper_sums(
    at_driver_langevin_zerofiller_t *zf,
    int ib_begin, int ib_end,
    at_mb_t *mb)
{
  return at_driver_langevin_zerofiller__fill_range_custom(
      zf, ib_begin, ib_end,
      at_driver_langevin_zerofiller__get_raw_bin_value_from_proper_sum,
      mb);
}



#endif
