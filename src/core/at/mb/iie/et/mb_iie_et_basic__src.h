#ifndef AT__MB_IIE_ET_BASIC__SRC_H__
#define AT__MB_IIE_ET_BASIC__SRC_H__



#include "mb_iie_et_basic.h"

#include "../../mb.h"
#include "../../../zcom/cfg/cfg.h"
#include "mb_iie_et_cache.h"




static void mb_iie_et_item__clear(mb_iie_et_item_t *item)
{
  item->value = 0.0;
  item->imbalance = 0.0;
  item->quality = 0;
  mb_iie_et_item_cache__clear(item->cache);
} 



void mb_iie_et__cfg_init(mb_iie_et_t *et, mb_t *mb, cfg_t *cfg, int silent)
{
  int i;

  et->n = mb->n;

  if ((et->items = (mb_iie_et_item_t *) calloc(et->n, sizeof(mb_iie_et_item_t))) == NULL) {
    fprintf(stderr, "no memory! var: et_items, type: mb_iie_et_item_t\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < et->n; i++) {
    mb_iie_et_item__clear(et->items + i);
  }

  et->cache_params->enabled = 1;
  if (cfg != NULL && 0 != cfg_get(cfg, &et->cache_params->enabled, "et_cache_enabled", "%d")) {
    if (!silent) {
      fprintf(stderr, "assuming default: mb->iie->et->cache_params->enabled = %d, key: et_cache_enabled\n",
          et->cache_params->enabled);
    }
  }

  et->cache_params->lifespan = 10.0;
  if (cfg != NULL && 0 != cfg_get(cfg, &et->cache_params->lifespan, "et_cache_lifespan", "%lf")) {
    if (!silent) {
      fprintf(stderr, "assuming default: mb->iie->et->cache_params->lifespan = %lf, key: et_cache_lifespan\n",
          et->cache_params->lifespan);
    }
  }

}



void mb_iie_et__finish(mb_iie_et_t *et)
{
  free(et->items);
}



void mb_iie_et__clear(mb_iie_et_t *et)
{
  int i;

  for (i = 0; i < et->n; i++) {
    mb_iie_et_item__clear(et->items + i);
  }

}



#endif
