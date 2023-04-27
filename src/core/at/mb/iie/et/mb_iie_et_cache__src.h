#ifndef AT__MB_IIE_ET_CACHE__SRC_H__
#define AT__MB_IIE_ET_CACHE__SRC_H__



#include "mb_iie_et_cache.h"
#include "../../accum/mb_accum.h"
#include "../../mb.h"
#include "../../../../zcom/zcom.h"



void at_mb_iie_et_item_cache__clear(at_mb_iie_et_item_cache_t *cache)
{
  cache->value = 0.0;
  cache->tag = 0.0;
  cache->expires = 0.0;
}


static double at_mb_iie_et_item_cache__get_tag(at_mb_iie_t *iie, int ib)
{
  return at_mb_accum__get_window_visits(iie->accum, ib);
}



static int at_mb_iie_et__cache_applicable(at_mb_iie_et_cache_params_t *cache_params)
{
  if (!cache_params->enabled) {
    return 0;
  }

  // If the plain accumulator is active, the window visits must be counted
  // from the plain accumulator bin by bin,
  // Consider to disable the cache in this case if necessary.
  //return iie->accum->use_winaccum;

  return 1;
}


static int at_mb_iie_et_item_cache__usable(
    at_mb_iie_et_item_t *item,
    at_mb_iie_t *iie,
    at_mb_iie_et_cache_params_t *cache_params,
    int ib)
{
  if (at_mb_iie_et__cache_applicable(cache_params)) {

    double tag, visits;

    tag = at_mb_iie_et_item_cache__get_tag(iie, ib);
    visits = tag;

    if (at_mb_iie_et__debug__ >= 2) {
      fprintf(stderr, "at_mb_iie_et_item_cache__usable(), %s:%d\n", __FILE__, __LINE__);
      fprintf(stderr, "  ib %d\n", ib);
      fprintf(stderr, "  item->cache->value %g\n", item->cache->value);
      fprintf(stderr, "  item->cache->tag %g, tag %g (visits)\n", item->cache->tag, tag);
      fprintf(stderr, "  item->cache->expires %g\n", item->cache->expires);
      fprintf(stderr, "\n");
    }

    // check if the raw data for the cache has changed
    if (tag == item->cache->tag && tag > 0) {
      return 1;
    }

    // if the raw data has changed, check if the cache data has expired or not
    if (visits < item->cache->expires && visits >= cache_params->min_visits) {
      return 1;
    }

  }

  return 0; // no cache for the plain accumulator
}


static int at_mb_iie_et_item_cache__deposit(
    at_mb_iie_et_item_t *item,
    at_mb_iie_t *iie,
    int ib)
{
  at_mb_iie_et_item_cache_t *cache = item->cache;

  //fprintf(stderr, "%d items %p\n", iie->accum->use_winaccum, iie->accum->winaccum->items);
  double tag = at_mb_iie_et_item_cache__get_tag(iie, ib);
  double expires = tag + iie->et->cache_params->lifespan;

  // the cache value can be something unsuccessful
  // as long as the tag is still be same
  cache->value = item->value;
  cache->tag = tag;
  cache->expires = expires;

  return 0;
}



double at_mb_iie_et__calc_et_cached(at_mb_iie_t *iie, int ib)
{
  at_mb_iie_et_t *et = iie->et;
  at_mb_iie_et_item_t *item = et->items + ib;

  zcom_util__exit_if (ib < 0 || ib >= iie->n,
      "bad ib %d [0, %d).\n", ib, iie->n);

  if (at_mb_iie_et_item_cache__usable(item, iie, iie->et->cache_params, ib)) { // use the cache value if possible

    et->cache_hit = 1;

    if (at_mb_iie_et__debug__ >= 1) {
      double et_val = at_mb_iie_et__calc_et(iie, ib);
      double tag = at_mb_iie_et_item_cache__get_tag(iie, ib);
      fprintf(stderr, "at_mb_iie_et__calc_et_cached(), %s:%d\n", __FILE__, __LINE__);
      fprintf(stderr, "  ib %d\n", ib);
      fprintf(stderr, "  item->cache->value %g vs. et_val %g\n", item->cache->value, et_val);
      fprintf(stderr, "  item->cache->tag %g, tag %g (visits)\n", item->cache->tag, tag);
      fprintf(stderr, "  item->cache->expires %g\n", item->cache->expires);
      fprintf(stderr, "\n");
    }

    return item->cache->value;

  } else {

    double et_val = at_mb_iie_et__calc_et(iie, ib);

    et->cache_hit = 0;

    if (at_mb_iie_et__cache_applicable(iie->et->cache_params)) {

      at_mb_iie_et_item_cache__deposit(item, iie, ib);

    }

    return et_val;
  }

}


#endif
