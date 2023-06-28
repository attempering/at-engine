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

#ifndef AT_MB_ACCUM_WINACCUM_ITEM__SRC_H__
#define AT_MB_ACCUM_WINACCUM_ITEM__SRC_H__


#include "at_mb_accum_winaccum_item.h"

#include "../../sm/at_mb_sm.h"

#include "../../../../zcom/zcom.h"



at_mb_sm_t *at_mb_accum_winaccum_item__get_sums(
    at_mb_accum_winaccum_item_t *item,
    int j)
{
  return item->sums + j - item->js;
}



void at_mb_accum_winaccum_item__normalize(
    at_mb_accum_winaccum_item_t *item)
{
  double fac;
  int j, m = item->jt - item->js;

  if (fabs(item->amplifier - 1.0) < 1e-6) {
    return;
  }

  fac = 1.0 / item->amplifier;

  for (j = 0; j < m; j++) {
    at_mb_sm__scale(item->sums + j, fac);
  }

  item->amplifier = 1.0;
}



double at_mb_accum_winaccum_item__calc_total(
    at_mb_accum_winaccum_item_t *item)
{
  double total = 0.0;
  int j, m = item->jt - item->js;

  for (j = 0; j < m; j++) {
    total += item->sums[j].s;
  }

  return total / item->amplifier;
}



void at_mb_accum_winaccum_item__add(
    at_mb_accum_winaccum_item_t *item,
    int ib,
    double invwf, double e, int do_2nd,
    at_mb_shk_t *shk,
    double total_visits)
{
  const double amplifier_threshold = 2.0;
  double ginvwf;
  at_mb_sm_t *sm;
  double inv_gamma;

  inv_gamma = at_mb_shk__calc_inv_gamma(shk, total_visits, item->i);

  //fprintf(stderr, "inv_gamma %g, shk %p\n", inv_gamma, shk); getchar();

  // apply adaptive averaging
  item->amplifier *= inv_gamma;

  ginvwf = item->amplifier * invwf; /* multiplied by the accumulated 1/gamma */

  sm = at_mb_accum_winaccum_item__get_sums(item, ib);

  //printf("%g %g %g %g\n", xsm->s, xsm->se, xsm->se2, xsm->se3);
  at_mb_sm__add(sm, ginvwf, e, do_2nd);

  /* we call normalization when the weight starts to blow up */
  if (item->amplifier > amplifier_threshold) {
    at_mb_accum_winaccum_item__normalize(item);
  }

  item->visits += 1;

}



void at_mb_accum_winaccum_item__init(
    at_mb_accum_winaccum_item_t *item,
    int i, at_mb_win_t *win)
{
  int m;
  int j;

  item->i = i;
  item->js = win->js_bin[i];
  item->jt = win->jt_bin[i];

  m = item->jt - item->js;
  if ((item->sums = (at_mb_sm_t *) calloc(m, sizeof(at_mb_sm_t))) == NULL) {
    fprintf(stderr, "Error@at.mb.accum.winaccum: no memory for winaccum item->sums\n");
    exit(1);
  }

  for (j = 0; j < m; j++) {
    at_mb_sm__init(item->sums + j);
  }

  item->amplifier = 1.0;
  item->visits = 0.0;
}



void at_mb_accum_winaccum_item__clear(
    at_mb_accum_winaccum_item_t *item)
{
  int j, m = item->jt - item->js;

  for (j = 0; j < m; j++) {
    at_mb_sm__clear(item->sums + j);
  }

  item->amplifier = 1.0;
  item->visits = 0.0;
}



void at_mb_accum_winaccum_item__finish(
    at_mb_accum_winaccum_item_t *item)
{
  free(item->sums);
}



int at_mb_accum_winaccum_item__read_binary(
    at_mb_accum_winaccum_item_t *item,
    const char *fn, FILE *fp, int version, int endn)
{
  int i = item->i, itmp, j;
  at_mb_sm_t *sm;

  // read the item index
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading item index %d, %s\n", i, fn);
    goto ERR;
  }

  if (itmp != i) {
    fprintf(stderr, "Error@at.mb.accum.winaccum: item indices mismatch, expect: %d, read: %d, pos: %#lx\n",
        i, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  if (version >= 3) {

    // read js
    if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading js for window %d\n",
          i);
      goto ERR;
    }

    if (itmp != item->js) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: window %d js mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, item->js, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    // read jt
    if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading js for window %d\n",
          i);
      goto ERR;
    }

    if (itmp != item->jt) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: window %d jt mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, item->jt, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    // read amplifier
    if (zcom_endn__fread(&item->amplifier, sizeof(double), 1, fp, endn) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading amplifier for window %d\n",
          i);
      goto ERR;
    }

    // read number of visits
    if (zcom_endn__fread(&item->visits, sizeof(double), 1, fp, endn) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading visits for window %d\n",
          i);
      goto ERR;
    }

  }

  for (j = item->js; j < item->jt; j++) {

    // read the bin index
    if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading bin index %d for window %d\n",
          j, i);
      goto ERR;
    }

    if (itmp != j) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: window %d, bin indices j mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, j, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    // read the sums
    sm = at_mb_accum_winaccum_item__get_sums(item, j);

    if (0 != at_mb_sm__read_binary(sm, fp, endn)) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error reading object winaccum->items->sums i %d j %d\n", i, j);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}



int at_mb_accum_winaccum_item__write_binary(
    at_mb_accum_winaccum_item_t *item,
    const char *fn, FILE *fp, int version)
{
  int i = item->i, j;
  at_mb_sm_t *sm;

  at_mb_accum_winaccum_item__normalize(item);

  // write the item index
  if (zcom_endn__fwrite(&i, sizeof(int), 1, fp, 1) != 1) {
    fprintf(stderr, "Error@at.mb.accum.winaccum: error in writing window index %d, %s\n",
        item->i, fn);
    goto ERR;
  }

  if (version >= 3) {

    // write js and jt
    if (zcom_endn__fwrite(&item->js, sizeof(int), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in writing js %d for window %d\n",
          item->js, item->i);
      goto ERR;
    }

    if (zcom_endn__fwrite(&item->jt, sizeof(int), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in writing jt %d for window %d\n",
          item->jt, item->i);
      goto ERR;
    }

    // write the amplifier
    if (zcom_endn__fwrite(&item->amplifier, sizeof(double), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in writing amplifier %g for window %d\n",
          item->amplifier, item->i);
      goto ERR;
    }

    // write the number of visits
    if (zcom_endn__fwrite(&item->visits, sizeof(double), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in writing visits %g for window %d\n",
          item->visits, item->i);
      goto ERR;
    }

  }

  for (j = item->js; j < item->jt; j++) {

    // write the bin index
    if (zcom_endn__fwrite(&j, sizeof(j), 1, fp, 1) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in writing j\n");
      goto ERR;
    }

    // write the accumulator data
    sm = at_mb_accum_winaccum_item__get_sums(item, j);

    if (0 != at_mb_sm__write_binary(sm, fp)) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error writing object winaccum->items->sums i %d j %d\n", i, j);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  return 0;
ERR:

  return -1;
}


int at_mb_accum_winaccum_item__read_text(
    at_mb_accum_winaccum_item_t *item,
    const char *fn, FILE *fp, int version)
{
  int i = item->i, itmp, j;
  at_mb_sm_t *sm;

  (void) version;

  // read the item index
  if (fscanf(fp, "%d", &itmp) != 1) {
    fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading item index %d, %s\n", i, fn);
    goto ERR;
  }

  if (itmp != i) {
    fprintf(stderr, "Error@at.mb.accum.winaccum: item indices mismatch, expect: %d, read: %d, pos: %#lx\n",
        i, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  {
    // read js
    if (fscanf(fp, "%d", &itmp) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading js for window %d\n",
          i);
      goto ERR;
    }

    if (itmp != item->js) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: window %d js mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, item->js, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    // read jt
    if (fscanf(fp, "%d", &itmp) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading js for window %d\n",
          i);
      goto ERR;
    }

    if (itmp != item->jt) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: window %d jt mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, item->jt, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    // read amplifier
    if (fscanf(fp, "%lf", &item->amplifier) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading amplifier for window %d\n",
          i);
      goto ERR;
    }

    // read number of visits
    if (fscanf(fp, "%lf", &item->visits) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading visits for window %d\n",
          i);
      goto ERR;
    }

  }

  for (j = item->js; j < item->jt; j++) {

    // read the bin index
    if (fscanf(fp, "%d", &itmp) != 1) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error in reading bin index %d for window %d\n",
          j, i);
      goto ERR;
    }

    if (itmp != j) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: window %d, bin indices j mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, j, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    // read the sums
    sm = at_mb_accum_winaccum_item__get_sums(item, j);

    if (0 != at_mb_sm__read_text(sm, fp)) {
      fprintf(stderr, "Error@at.mb.accum.winaccum: error reading object winaccum->items->sums i %d j %d\n", i, j);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}



int at_mb_accum_winaccum_item__write_text(
    at_mb_accum_winaccum_item_t *item,
    const char *fn, FILE *fp, int version)
{
  int j;
  at_mb_sm_t *sm;

  (void) version;

  at_mb_accum_winaccum_item__normalize(item);

  // write the item index, js, jt, amplifier, visits
  fprintf(fp, "%d %d %d %.15e %.15e\n",
      item->i, item->js, item->jt, item->amplifier, item->visits);

  for (j = item->js; j < item->jt; j++) {

    // write the bin index
    fprintf(fp, "%d ", j);

    // write the accumulator data
    sm = at_mb_accum_winaccum_item__get_sums(item, j);

    at_mb_sm__write_text(sm, fp);
  }

  return 0;
}



#endif
