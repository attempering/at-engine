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

#ifndef AT_MB_IIE_ET_CACHE__SRC_H__
#define AT_MB_IIE_ET_CACHE__SRC_H__



#include "at_mb_iie_et_cache.h"
#include "../../accum/at_mb_accum.h"
#include "../../at_mb.h"
#include "../../../utils/at_utils.h"
#include "../../../../zcom/zcom.h"



void at_mb_iie_et_item_cache__clear(at_mb_iie_et_item_cache_t *cache)
{
  //cache->value = 0.0;
  cache->visits = 0.0;
  cache->expires = 0.0;
}


static double at_mb_iie_et_item_cache__get_visits(at_mb_iie_t *iie, int ib)
{
  return at_mb_accum__get_window_visits(iie->accum, ib);
}



static int at_mb_iie_et__cache_applicable(at_mb_iie_et_cache_params_t *cache_params)
{
  return cache_params->enabled;

  // If the plain accumulator is active, the window visits must be counted
  // from the plain accumulator bin by bin,
  // Consider to disable the cache in this case if necessary.
  //return iie->accum->winaccum->enabled;

}


/*
 * Cache use policy
 *
 * A previously calculated value is usable if
 *
 * 1. Cache is turned on, i.e., cache_params->enabled is true
 *
 * 2. One of the following conditions is satisfied
 *
 *   2.a. The number of visits from the accumulator has not been changed.
 *        In this case, we can of course use the previously calculated result.
 *        Note that this does not mean the cached value of good quality.
 *        If the number of visits is 0.0, the cached value item->value is also 0.0.
 *        We can use the cached value of 0.0, which is of bad quality
 *        item->quality = 0.0.
 * 
 *   2.b. The number of visits has changed, but 
 * 
 *      2.b.I. It hasn't exceed the expiration, the quantity given by `expires`
 *      2.b.II. AND it is not less than the minimum visits threshold, `min_visits`
 *
 */
static at_bool_t at_mb_iie_et_item_cache__usable(
    at_mb_iie_et_item_t *item,
    at_mb_iie_t *iie,
    at_mb_iie_et_cache_params_t *cache_params,
    int ib,
    double *p_visits)
{
  if (at_mb_iie_et__cache_applicable(cache_params)) {

    double visits;

    visits = at_mb_iie_et_item_cache__get_visits(iie, ib);
    *p_visits = visits;

    if (at_mb_iie_et__debug__ >= 2) {
      fprintf(stderr, "at_mb_iie_et_item_cache__usable(), %s:%d\n", __FILE__, __LINE__);
      fprintf(stderr, "  ib %d\n", ib);
      //fprintf(stderr, "  item->cache->value %g\n", item->cache->value);
      fprintf(stderr, "  item->cache->visits %g, calc. visits %g\n", item->cache->visits, visits);
      fprintf(stderr, "  item->cache->expires %g\n", item->cache->expires);
      fprintf(stderr, "\n");
    }

    // Condition 2.a.: the number of visits has not changed
    if (visits == item->cache->visits && visits > 0) {
      return AT__TRUE;
    }

    // Condition 2.b.: the number of visits has changed,
    // but is within the expiration date and is at least min_visits.
    if (visits < item->cache->expires && visits >= cache_params->min_visits) {
      return AT__TRUE;
    } else {
      // fall through and return false
      //printf("cache %d miss %g expires %g %g\n", item->ib, visits, item->cache->expires, cache_params->min_visits);
    }

  } else {
    *p_visits = 0.0;
  }

  return AT__FALSE; // no cache for the plain accumulator
}


static int at_mb_iie_et_item_cache__deposit(
    at_mb_iie_et_item_t *item,
    at_mb_iie_t *iie,
    int ib,
    double visits)
{
  at_mb_iie_et_item_cache_t *cache = item->cache;

  // This function is called after at_mb_iie_et__calc_et_cached()
  // in at_mb_iie_et__calc_et_cached()
  //
  // The number of visits is sometimes set in at_mb_iie_et__calc_et_cached()
  // but it is not always so.
  //
  // For example, if the cache functionality is turned off
  // and at_mb_iie_et__cache_applicable() is false
  // then the value is 0.0 regardless of the actual number of visits.
  //
  // Therefore, we need to recalculated number of visits
  // when a zero value is encountered.
  //
  if (visits == 0.0) {
    visits = at_mb_iie_et_item_cache__get_visits(iie, ib);
  }

  //fprintf(stderr, "%d items %p\n", iie->accum->winaccum->enabled, iie->accum->winaccum->items);
  double expires = visits + iie->et->cache_params->lifespan;

  // the cache value can be something unsuccessful
  // as long as the visits is still be same
  //cache->value = item->value;
  cache->visits = visits;
  cache->expires = expires;

  return 0;
}



double at_mb_iie_et__calc_et_cached(at_mb_iie_t *iie, int ib, int *quality)
{
  at_mb_iie_et_t *et = iie->et;
  at_mb_iie_et_item_t *item = et->items + ib;
  double visits = 0;

  zcom_util__exit_if (ib < 0 || ib >= iie->n,
      "bad ib %d [0, %d).\n", ib, iie->n);

  if (at_mb_iie_et_item_cache__usable(item, iie, iie->et->cache_params, ib, &visits)) { // use the cache value if possible

    et->cache_hit = 1;

    if ((ib == 0 || ib == et->n - 1) && item->value == 0.0) {
      fprintf(stderr, "at-warning: cache hit, ib %d but value is zero\n", ib);
      getchar();
    }

    if (at_mb_iie_et__debug__ >= 1) {
      double et_val = 0;
      // for testing, set
      // et_val = at_mb_iie_et__calc_et(iie, ib);
      double visits = at_mb_iie_et_item_cache__get_visits(iie, ib);
      fprintf(stderr, "at_mb_iie_et__calc_et_cached(), %s:%d\n", __FILE__, __LINE__);
      fprintf(stderr, "  ib %d\n", ib);
      fprintf(stderr, "  item->value %g vs. et_val %g\n", item->value, et_val);
      fprintf(stderr, "  item->cache->visits %g, calc. visits %g\n", item->cache->visits, visits);
      fprintf(stderr, "  item->cache->expires %g\n", item->cache->expires);
      fprintf(stderr, "\n");
    }

    if (quality != NULL) {
      *quality = item->quality;
    }

    return item->value;

  } else {

    // the function at_mb_iie_et__calc_et()
    // also save the result in item->value,
    // and the quality bit in *quality == item->quality
    double et_val = at_mb_iie_et__calc_et(iie, ib, quality);

    et->cache_hit = 0;

    //if ((ib == 0 || ib == et->n - 1) && item->cache->value == 0.0) {
    //  fprintf(stderr, "at-info: cache miss, ib %d but value is zero, item->quality %d\n", ib, item->quality);
    //  getchar();
    //}

    if (at_mb_iie_et__cache_applicable(iie->et->cache_params)) {

      at_mb_iie_et_item_cache__deposit(item, iie, ib, visits);

    }

    return et_val;
  }

}


#endif
