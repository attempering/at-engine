#ifndef AT__MB_IIE_GRIDVALS_BASIC__SRC_H__
#define AT__MB_IIE_GRIDVALS_BASIC__SRC_H__



/* integral-identity estimators for grid point values */



#include "mb_iie_gridvals_basic.h"


static void mb_iie_gridvals_item__clear(mb_iie_gridvals_item_t *item)
{
  item->lnz = 0.0;
  item->e = 0.0;
  item->cv = 0.0;
  item->quality_bits = 0;
}



void mb_iie_gridvals__cfg_init(mb_iie_gridvals_t *gridvals, mb_t *mb,
    cfg_t *cfg, int silent)
{
  int i, n = mb->n;

  gridvals->n = n;

  gridvals->items = NULL;
  if ((gridvals->items = (mb_iie_gridvals_item_t *) calloc((n + 1), sizeof(mb_iie_gridvals_item_t))) == NULL) {
    fprintf(stderr, "no memory! var: mb->iie->gridvals->items, type: mb_iie_gridvals_item_t\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  for (i = 0; i <= n; i++) {
    mb_iie_gridvals_item__clear(gridvals->items + i);
  }
}


void mb_iie_gridvals__clear(mb_iie_gridvals_t *gridvals)
{
  int i, n = gridvals->n;

  for (i = 0; i <= n; i++) {
    mb_iie_gridvals_item__clear(gridvals->items + i);
  }

}



void mb_iie_gridvals__finish(mb_iie_gridvals_t *gridvals)
{
  free(gridvals->items);
}


#endif
