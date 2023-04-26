#ifndef AT__MB_ACCUM_WINACCUM_ITEM__SRC_H__
#define AT__MB_ACCUM_WINACCUM_ITEM__SRC_H__


#include "mb_accum_winaccum_item.h"

#include "../../sm/mb_sm.h"

#include "../../../zcom/util/util.h"
#include "../../../zcom/cfg/cfg.h"
#include "../../../zcom/endn/endn.h"



at_mb_sm_t *at_mb_accum_winaccum_item__get_sums(
    at_mb_accum_winaccum_item_t *item,
    int j)
{
  return item->sums + j - item->js;
}



void at_mb_accum_winaccum_item__normalize(at_mb_accum_winaccum_item_t *item)
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



double at_mb_accum_winaccum_item__calc_total(at_mb_accum_winaccum_item_t *item)
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

  // apply adaptive averaging
  item->amplifier *= at_mb_shk__calc_inverse_gamma(shk, total_visits, item->i);

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



void at_mb_accum_winaccum_item__init(at_mb_accum_winaccum_item_t *item,
    int i, at_mb_win_t *win)
{
  int m;
  int j;

  item->i = i;
  item->js = win->js_bin[i];
  item->jt = win->jt_bin[i];

  m = item->jt - item->js;
  if ((item->sums = (at_mb_sm_t *) calloc(m, sizeof(at_mb_sm_t))) == NULL) {
    fprintf(stderr, "no memory for winaccum item->sums\n");
    exit(1);
  }

  for (j = 0; j < m; j++) {
    at_mb_sm__init(item->sums + j);
  }

  item->amplifier = 1.0;
  item->visits = 0.0;
}



void at_mb_accum_winaccum_item__clear(at_mb_accum_winaccum_item_t *item)
{
  int j, m = item->jt - item->js;

  for (j = 0; j < m; j++) {
    at_mb_sm__clear(item->sums + j);
  }

  item->amplifier = 1.0;
  item->visits = 0.0;
}



void at_mb_accum_winaccum_item__finish(at_mb_accum_winaccum_item_t *item)
{
  free(item->sums);
}



int at_mb_accum_winaccum_item__read_binary(at_mb_accum_winaccum_item_t *item, FILE *fp, int endn)
{
  int i = item->i, itmp, j;
  at_mb_sm_t *sm;

  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != i) {
    fprintf(stderr, "i mismatch, expect: %d, read: %d, pos: %#lx\n",
        i, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  for (j = item->js; j < item->jt; j++) {
    if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading itmp\n");
      goto ERR;
    }
    if (itmp != j) {
      fprintf(stderr, "j mismatch, expect: %d, read: %d, pos: %#lx\n",
          j, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    sm = at_mb_accum_winaccum_item__get_sums(item, j);

    if (0 != at_mb_sm__read_binary(sm, fp, endn)) {
      fprintf(stderr, "error reading object winaccum->items->sums i %d j %d\n", i, j);
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  return 0;

ERR:

  return -1;
}



int at_mb_accum_winaccum_item__write_binary(at_mb_accum_winaccum_item_t *item, FILE *fp)
{
  int i = item->i, j;
  at_mb_sm_t *sm;

  at_mb_accum_winaccum_item__normalize(item);

  if (zcom_endn__fwrite(&i, sizeof(int), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing i\n");
    goto ERR;
  }

  for (j = item->js; j < item->jt; j++) {
    if (zcom_endn__fwrite(&j, sizeof(j), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing j\n");
      goto ERR;
    }

    sm = at_mb_accum_winaccum_item__get_sums(item, j);

    if (0 != at_mb_sm__write_binary(sm, fp)) {
      fprintf(stderr, "error writing object winaccum->items->sums i %d j %d\n", i, j);
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  return 0;
ERR:

  return -1;
}


#endif
