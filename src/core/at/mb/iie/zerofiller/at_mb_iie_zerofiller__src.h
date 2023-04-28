/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

#ifndef AT_MB_IIE_ZEROFILLER__SRC_H__
#define AT_MB_IIE_ZEROFILLER__SRC_H__



#include "at_mb_iie_zerofiller.h"
#include "../../at_mb_basic.h"


static void at_mb_iie_zerofiller_item__set_zero(at_mb_iie_zerofiller_item_t *p)
{
  p->w = 0;
  p->mean = 0;
  p->var = 0;
}


static void at_mb_iie_zerofiller_item__copy(at_mb_iie_zerofiller_item_t *dest, const at_mb_iie_zerofiller_item_t *src)
{
  *dest = *src;
}

static void at_mb_iie_zerofiller_item__set_equal(
    at_mb_iie_zerofiller_item_t *c,
    const at_mb_iie_zerofiller_item_t *a,
    const at_mb_iie_zerofiller_item_t *b)
{
  /*
    // this implementation is abandoned.
    // Taking a weighted mean would be fine for the mean value,
    // but it makes little sense for the variance?
    double s = a->w + b->w;
    double wa = a->w / s;
    double wb = b->w / s;

    c->mean = (wa * a->mean + wb * b->var);
    c->var = (wa * a->var + wb * b->var);
  */

  if (a->w > b->w) {
    at_mb_iie_zerofiller_item__copy(c, a);
  } else if (a->w < b->w) {
    at_mb_iie_zerofiller_item__copy(c, b);
  } else {
    c->mean = (a->mean + b->mean) / 2;
    c->var = (a->var + b->var) / 2;
  }
}



void at_mb_iie_zerofiller__init(at_mb_iie_zerofiller_t *zf, at_mb_t *mb)
{
  int n = mb->n, i;

  if ((zf->vals = (at_mb_iie_zerofiller_item_t *) calloc((n + 1),
                  sizeof(at_mb_iie_zerofiller_item_t))) == NULL) {
    fprintf(stderr, "no memory! var: zf->vals, type: double\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    at_mb_iie_zerofiller_item__set_zero(&zf->vals[i]);
  }

  if ((zf->has_vals = (int *) calloc((n + 1), sizeof(int))) == NULL) {
    fprintf(stderr, "no memory! var: zf->has_vals, type: int\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    zf->has_vals[i] = 0;
  }


  if ((zf->ib_from_left = (int *) calloc((n + 1), sizeof(int))) == NULL) {
    fprintf(stderr, "no memory! var: zf->ib_from_left, type: int\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    zf->ib_from_left[i] = 0;
  }


  if ((zf->ib_from_right = (int *) calloc((n + 1), sizeof(int))) == NULL) {
    fprintf(stderr, "no memory! var: zf->ib_from_right, type: int\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    zf->ib_from_right[i] = 0;
  }


  zf->n = mb->n;
  zf->beta_min = mb->bmin;
  zf->beta_max = mb->bmax;
  zf->beta_del = mb->bdel;

  zf->mb = mb;
}



void at_mb_iie_zerofiller__finish(at_mb_iie_zerofiller_t *zf)
{
  free(zf->vals);
  free(zf->has_vals);
  free(zf->ib_from_left);
  free(zf->ib_from_right);
}



int at_mb_iie_zerofiller__fill_missing(at_mb_iie_zerofiller_t *zf, int ib_begin, int ib_end)
{
  int ib;
  int n = zf->n;
  int fill_errors = 0;
  const int dist_infinity = n * 10;

  int print_debug_info = 0;

  if (print_debug_info) {

    fprintf(stderr, "mb_iie_zerofiller: [%d, %d], unfilled:\n", ib_begin, ib_end);

    for (ib = ib_begin; ib <= ib_end; ib++) {
      fprintf(stderr, "ib %d, has %d, vals %g:%g,%g\n", ib, zf->has_vals[ib],
          zf->vals[ib].w, zf->vals[ib].mean, zf->vals[ib].var);
    }

    fprintf(stderr, "\n");

  }


  // left-to-right scan
  {
    int ib_from_left = -n*2; // far left

    for (ib = ib_begin; ib <= ib_end; ib++) {

      if (zf->has_vals[ib]) {
        // register the last good position from the left
        ib_from_left = ib;
      } else {
        // apply the last good position from the left
        zf->ib_from_left[ib] = ib_from_left;
      }

    }
  }


  // right-to-left scan
  {
    int ib_from_right = n*3; // far right

    for (ib = ib_end; ib >= ib_begin; ib--) {

      if (zf->has_vals[ib]) {
        // register the last good position from the right
        ib_from_right = ib;
      } else {
        // apply the last good position from the right
        zf->ib_from_right[ib] = ib_from_right;
      }

    }
  }


  // selection

  for (ib = ib_begin; ib <= ib_end; ib++) {

    if (!zf->has_vals[ib]) {

      int ib_from_left = zf->ib_from_left[ib];
      int dist_from_left = (ib_from_left >= 0 ? ib - ib_from_left : dist_infinity);

      int ib_from_right = zf->ib_from_right[ib];
      int dist_from_right = (ib_from_right >= 0 ? ib_from_right - ib : dist_infinity);

      if (dist_from_left < dist_from_right) {

        // use the left value
        at_mb_iie_zerofiller_item__copy(&zf->vals[ib],
          &zf->vals[ib_from_left]);

      } else if (dist_from_left > dist_from_right) {

        // use the right value
        at_mb_iie_zerofiller_item__copy(&zf->vals[ib],
          &zf->vals[ib_from_right]);

      } else { // dist_from_left == dist_from_right

        // use the mean of the values from the left and right
        if (ib_from_left >= 0 && ib_from_right >= 0) {
          at_mb_iie_zerofiller_item__set_equal(&zf->vals[ib],
            &zf->vals[ib_from_left],
            &zf->vals[ib_from_right]);
        } else {
          fill_errors++;
        }

      }

    }

    if (print_debug_info) {
      int ib_from_left = zf->ib_from_left[ib];
      int ib_from_right = zf->ib_from_right[ib];
      if (ib_from_left >= 0 && ib_from_right < n) {
        int dist_from_left = ib - zf->ib_from_left[ib];
        int dist_from_right = zf->ib_from_right[ib] - ib;
        printf("ib %d, has_vals %d, vals %g:%g,%g, "
              "vals (from left) %g:%g,%g,%d, "
              "vals (from right) %g:%g,%g,%d\n",
            ib, zf->has_vals[ib],
            zf->vals[ib].w,
            zf->vals[ib].mean,
            zf->vals[ib].var,
            zf->vals[ib_from_left].w,
            zf->vals[ib_from_left].mean,
            zf->vals[ib_from_left].var,
            dist_from_left,
            zf->vals[ib_from_right].w,
            zf->vals[ib_from_right].mean,
            zf->vals[ib_from_right].var,
            dist_from_right);
      }
    }

  }

  if (print_debug_info) {
    getchar();
  }

  return fill_errors;
}




#endif