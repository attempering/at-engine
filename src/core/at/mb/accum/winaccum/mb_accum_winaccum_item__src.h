#ifndef AT__MB_ACCUM_WINACCUM_ITEM__SRC_H__
#define AT__MB_ACCUM_WINACCUM_ITEM__SRC_H__


#include "mb_accum_winaccum_item.h"

#include "../../../sm/sm.h"

#include "../../../zcom/util/util.h"
#include "../../../zcom/cfg/cfg.h"
#include "../../../zcom/endn/endn.h"



sm_t *mb_accum_winaccum_item__get_sums(
    mb_accum_winaccum_item_t *item,
    int j)
{
  return item->sums + j - item->js;
}



void mb_accum_winaccum_item__normalize(mb_accum_winaccum_item_t *item)
{
  double fac;
  int j, m = item->jt - item->js;

  if (fabs(item->amplifier - 1.0) < 1e-6) {
    return;
  }

  fac = 1.0 / item->amplifier;

  for (j = 0; j < m; j++) {
    sm__scale(item->sums + j, fac);
  }

  item->amplifier = 1.0;
}



double mb_accum_winaccum_item__calc_total(mb_accum_winaccum_item_t *item)
{
  double total = 0.0;
  int j, m = item->jt - item->js;

  for (j = 0; j < m; j++) {
    total += item->sums[j].s;
  }

  return total / item->amplifier;
}



void mb_accum_winaccum_item__add(
    mb_accum_winaccum_item_t *item,
    int ib,
    double invwf, double e, int do_2nd,
    mb_shk_t *shk,
    double total_visits)
{
  const double amplifier_threshold = 2.0;
  double ginvwf;
  sm_t *sm;

  // apply adaptive averaging
  item->amplifier *= mb_shk__calc_inverse_gamma(shk, total_visits, item->i);

  ginvwf = item->amplifier * invwf; /* multiplied by the accumulated 1/gamma */

  sm = mb_accum_winaccum_item__get_sums(item, ib);

  //printf("%g %g %g %g\n", xsm->s, xsm->se, xsm->se2, xsm->se3);
  sm__add(sm, ginvwf, e, do_2nd);

  /* we call normalization when the weight starts to blow up */
  if (item->amplifier > amplifier_threshold) {
    mb_accum_winaccum_item__normalize(item);
  }

  item->visits += 1;

}



void mb_accum_winaccum_item__init(mb_accum_winaccum_item_t *item,
    int i, mb_win_t *win)
{
  int m;
  int j;

  item->i = i;
  item->js = win->js_bin[i];
  item->jt = win->jt_bin[i];

  m = item->jt - item->js;
  if ((item->sums = (sm_t *) calloc(m, sizeof(sm_t))) == NULL) {
    fprintf(stderr, "no memory for winaccum item->sums\n");
    exit(1);
  }

  for (j = 0; j < m; j++) {
    sm__init(item->sums + j);
  }

  item->amplifier = 1.0;
  item->visits = 0.0;
}



void mb_accum_winaccum_item__clear(mb_accum_winaccum_item_t *item)
{
  int j, m = item->jt - item->js;

  for (j = 0; j < m; j++) {
    sm__clear(item->sums + j);
  }

  item->amplifier = 1.0;
  item->visits = 0.0;
}



void mb_accum_winaccum_item__finish(mb_accum_winaccum_item_t *item)
{
  free(item->sums);
}



int mb_accum_winaccum_item__read_binary(mb_accum_winaccum_item_t *item, FILE *fp, int endn)
{
  int i = item->i, itmp, j;
  sm_t *sm;

  if (endn_fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
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
    if (endn_fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading itmp\n");
      goto ERR;
    }
    if (itmp != j) {
      fprintf(stderr, "j mismatch, expect: %d, read: %d, pos: %#lx\n",
          j, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    sm = mb_accum_winaccum_item__get_sums(item, j);

    if (0 != sm__read_binary(sm, fp, endn)) {
      fprintf(stderr, "error reading object winaccum->items->sums i %d j %d\n", i, j);
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  return 0;

ERR:

  return -1;
}



int mb_accum_winaccum_item__write_binary(mb_accum_winaccum_item_t *item, FILE *fp)
{
  int i = item->i, j;
  sm_t *sm;

  mb_accum_winaccum_item__normalize(item);

  if (endn_fwrite(&i, sizeof(int), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing i\n");
    goto ERR;
  }

  for (j = item->js; j < item->jt; j++) {
    if (endn_fwrite(&j, sizeof(j), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing j\n");
      goto ERR;
    }

    sm = mb_accum_winaccum_item__get_sums(item, j);

    if (0 != sm__write_binary(sm, fp)) {
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
