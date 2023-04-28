#ifndef AT_MB_IIE_GRIDVALS_BASIC__SRC_H__
#define AT_MB_IIE_GRIDVALS_BASIC__SRC_H__



/* integral-identity estimators for grid point values */



#include "at_mb_iie_gridvals_basic.h"


static void at_mb_iie_gridvals_item__clear(at_mb_iie_gridvals_item_t *item)
{
  item->lnz = 0.0;
  item->e = 0.0;
  item->cv = 0.0;
  item->quality_bits = 0;
}



void at_mb_iie_gridvals__cfg_init(at_mb_iie_gridvals_t *gridvals, at_mb_t *mb,
    zcom_cfg_t *cfg, int silent)
{
  int i, n = mb->n;

  gridvals->n = n;

  gridvals->items = NULL;
  if ((gridvals->items = (at_mb_iie_gridvals_item_t *) calloc((n + 1), sizeof(at_mb_iie_gridvals_item_t))) == NULL) {
    fprintf(stderr, "no memory! var: mb->iie->gridvals->items, type: at_mb_iie_gridvals_item_t\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  for (i = 0; i <= n; i++) {
    at_mb_iie_gridvals_item__clear(gridvals->items + i);
  }
}


void at_mb_iie_gridvals__clear(at_mb_iie_gridvals_t *gridvals)
{
  int i, n = gridvals->n;

  for (i = 0; i <= n; i++) {
    at_mb_iie_gridvals_item__clear(gridvals->items + i);
  }

}



void at_mb_iie_gridvals__finish(at_mb_iie_gridvals_t *gridvals)
{
  free(gridvals->items);
}


#endif
