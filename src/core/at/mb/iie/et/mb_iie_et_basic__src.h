#ifndef AT__MB_IIE_ET_BASIC__SRC_H__
#define AT__MB_IIE_ET_BASIC__SRC_H__



#include "mb_iie_et_basic.h"

#include "../../mb.h"
#include "../../../zcom/cfg/cfg.h"
#include "mb_iie_et_cache.h"


double at_mb_iie_et__debug__ = 0;


static void at_mb_iie_et_item__clear(at_mb_iie_et_item_t *item)
{
  item->value = 0.0;
  item->imbalance = 0.0;
  item->quality = 0;
  at_mb_iie_et_item_cache__clear(item->cache);
} 



void at_mb_iie_et__cfg_init(at_mb_iie_et_t *et, at_mb_t *mb, zcom_cfg_t *cfg, int silent)
{
  int i;

  et->n = mb->n;

  if ((et->items = (at_mb_iie_et_item_t *) calloc(et->n, sizeof(at_mb_iie_et_item_t))) == NULL) {
    fprintf(stderr, "no memory! var: et_items, type: at_mb_iie_et_item_t\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < et->n; i++) {
    at_mb_iie_et_item__clear(et->items + i);
  }

  et->cache_params->enabled = 1;
  if (cfg != NULL && 0 != zcom_cfg__get(cfg, &et->cache_params->enabled, "et_cache_enabled", "%d")) {
    if (!silent) {
      fprintf(stderr, "assuming default: mb->iie->et->cache_params->enabled = %d, key: et_cache_enabled\n",
          et->cache_params->enabled);
    }
  }

  et->cache_params->lifespan = 10.0;
  if (cfg != NULL && 0 != zcom_cfg__get(cfg, &et->cache_params->lifespan, "et_cache_lifespan", "%lf")) {
    if (!silent) {
      fprintf(stderr, "assuming default: mb->iie->et->cache_params->lifespan = %lf, key: et_cache_lifespan\n",
          et->cache_params->lifespan);
    }
  }

  et->cache_params->min_visits = 100.0;
  if (cfg != NULL && 0 != zcom_cfg__get(cfg, &et->cache_params->min_visits, "et_cache_min_visits", "%lf")) {
    if (!silent) {
      fprintf(stderr, "assuming default: mb->iie->et->cache_params->min_visits = %lf, key: et_cache_min_visits\n",
          et->cache_params->min_visits);
    }
  }

}



void at_mb_iie_et__finish(at_mb_iie_et_t *et)
{
  free(et->items);
}



void at_mb_iie_et__clear(at_mb_iie_et_t *et)
{
  int i;

  for (i = 0; i < et->n; i++) {
    at_mb_iie_et_item__clear(et->items + i);
  }

}



void at_mb_iie_et__manifest(const at_mb_iie_et_t *et, FILE *fp, int arrmax)
{
  fprintf(fp, "mb->iie->et->cache_params->enabled: double, %d\n", et->cache_params->enabled);

  fprintf(fp, "mb->iie->et->cache_params->lifespan: double, %g\n", et->cache_params->lifespan);

  fprintf(fp, "mb->iie->et->cache_params->min_visits: double, %g\n", et->cache_params->min_visits);
}


#endif
