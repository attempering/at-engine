#ifndef AT__MB_IIE_ET_CACHE__SRC_H__
#define AT__MB_IIE_ET_CACHE__SRC_H__



#include "mb_iie_et_cache.h"
#include "../../accum/mb_accum.h"
#include "../../mb.h"
#include "../../../zcom/util/util.h"



void mb_iie_et_item_cache__clear(mb_iie_et_item_cache_t *cache)
{
  cache->value = 0.0;
  cache->tag = 0.0;
  cache->expires = 0.0;
}


static double mb_iie_et_item_cache__get_tag(mb_iie_t *iie, int ib)
{
  return mb_accum__get_window_visits(iie->accum, ib);
}



static int mb_iie_et__cache_applicable(mb_iie_t *iie)
{
  if (!iie->et->cache_params->enabled) {
    return 0;
  }

  // If the plain accumulator is active, the window visits must be counted
  // from the plain accumulator bin by bin,
  // Consider to disable the cache in this case if necessary.
  //return iie->accum->use_winaccum;

  return 1;
}


static int mb_iie_et_item_cache__usable(mb_iie_et_item_t *item, mb_iie_t *iie, int ib)
{
  if (mb_iie_et__cache_applicable(iie)) {

    double tag, visits;
    
    tag = mb_iie_et_item_cache__get_tag(iie, ib);

    if (tag == item->cache->tag) {
      return 1;
    }

    visits = tag;
    if (visits < item->cache->expires) {
      return 1;
    }

  }

  return 0; // no cache for the plain accumulator
}


static int mb_iie_et_item_cache__deposit(
    mb_iie_et_item_t *item,
    mb_iie_t *iie,
    int ib)
{
  mb_iie_et_item_cache_t *cache = item->cache;

  //fprintf(stderr, "%d items %p\n", iie->accum->use_winaccum, iie->accum->winaccum->items);
  double tag = mb_iie_et_item_cache__get_tag(iie, ib);
  double expires = tag + iie->et->cache_params->lifespan;

  // the cache value can be something unsuccessful
  // as long as the tag is still be same
  cache->tag = tag;
  cache->expires = expires;

  return 0;
}



double mb_iie_et__calc_et_cached(mb_iie_t *iie, int ib)
{
  mb_iie_et_t *et = iie->et;
  mb_iie_et_item_t *item = et->items + ib;

  exit_if (ib < 0 || ib >= iie->n,
      "bad ib %d [0, %d).\n", ib, iie->n);

  if (mb_iie_et_item_cache__usable(item, iie, ib)) { // use the cache value if possible

    return item->cache->value;

  } else {

    double et_val = mb_iie_et__calc_et(iie, ib);

    if (mb_iie_et__cache_applicable(iie)) {

      mb_iie_et_item_cache__deposit(item, iie, ib);

    }


    return et_val;
  }

}


#endif
