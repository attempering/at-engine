#ifndef AT__MB_IIE_ZEROFILLER__SRC_H__
#define AT__MB_IIE_ZEROFILLER__SRC_H__



#include "mb_iie_zerofiller.h"
#include "../../mb_basic.h"


static void mb_iie_zerofiller_item__set_zero(mb_iie_zerofiller_item_t *p)
{
  p->w = 0;
  p->mean = 0;
  p->var = 0;
}


static void mb_iie_zerofiller_item__copy(mb_iie_zerofiller_item_t *dest, const mb_iie_zerofiller_item_t *src)
{
  *dest = *src;
}

static void mb_iie_zerofiller_item__set_equal(
    mb_iie_zerofiller_item_t *c,
    const mb_iie_zerofiller_item_t *a,
    const mb_iie_zerofiller_item_t *b)
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
    mb_iie_zerofiller_item__copy(c, a);
  } else if (a->w < b->w) {
    mb_iie_zerofiller_item__copy(c, b);
  } else {
    c->mean = (a->mean + b->mean) / 2;
    c->var = (a->var + b->var) / 2;
  }
}



void mb_iie_zerofiller__init(mb_iie_zerofiller_t *zf, mb_t *mb)
{
  int n = mb->n, i;

  if ((zf->vals = (mb_iie_zerofiller_item_t *) calloc((n + 1),
                  sizeof(mb_iie_zerofiller_item_t))) == NULL) {
    fprintf(stderr, "no memory! var: zf->vals, type: double\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < n+1; i++) {
    mb_iie_zerofiller_item__set_zero(&zf->vals[i]);
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



void mb_iie_zerofiller__finish(mb_iie_zerofiller_t *zf)
{
  free(zf->vals);
  free(zf->has_vals);
  free(zf->ib_from_left);
  free(zf->ib_from_right);
}



int mb_iie_zerofiller__fill_missing(mb_iie_zerofiller_t *zf, int ib_begin, int ib_end)
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
        mb_iie_zerofiller_item__copy(&zf->vals[ib],
          &zf->vals[ib_from_left]);

      } else if (dist_from_left > dist_from_right) {

        // use the right value
        mb_iie_zerofiller_item__copy(&zf->vals[ib],
          &zf->vals[ib_from_right]);

      } else { // dist_from_left == dist_from_right

        // use the mean of the values from the left and right
        if (ib_from_left >= 0 && ib_from_right >= 0) {
          mb_iie_zerofiller_item__set_equal(&zf->vals[ib],
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