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

#ifndef AT_MB_IIE_ET_BASIC__SRC_H__
#define AT_MB_IIE_ET_BASIC__SRC_H__



#include "at_mb_iie_et_basic.h"

#include "../../at_mb.h"
#include "../../../../zcom/zcom.h"
#include "at_mb_iie_et_cache.h"


int at_mb_iie_et__debug__ = 0;


static void at_mb_iie_et_item__clear(at_mb_iie_et_item_t *item)
{
  item->value = 0.0;
  item->imbalance = 0.0;
  item->quality = 0;
  at_mb_iie_et_item_cache__clear(item->cache);
} 



void at_mb_iie_et__conf_init(at_mb_iie_et_t *et, at_mb_t *mb, at_utils_conf_t *conf)
{
  int i;

  at_utils_conf__push_mod(conf, "at.mb.iie.et");

  et->n = mb->distr->domain->n;

  et->items = (at_mb_iie_et_item_t *) calloc(et->n, sizeof(at_mb_iie_et_item_t));

  at_utils_log__exit_if (et->items == NULL,
      conf->log, "no memory! var: et_items, type: at_mb_iie_et_item_t\n");

  for (i = 0; i < et->n; i++) {
    at_mb_iie_et_item__clear(et->items + i);
    et->items[i].ib = i;
  }

  at_utils_conf__get_bool(conf,
      "et-cache-enabled",
      &et->cache_params->enabled, AT__FALSE,
      "cached_enabled");

  if (et->cache_params->enabled) {

    at_utils_conf__get_double(conf,
        "et-cache-lifespan",
        &et->cache_params->lifespan, 10.0,
        "lifespan");

    at_utils_conf__get_double(conf,
        "et-cache-min-visits",
        &et->cache_params->min_visits, 100.0,
        "min_visits");

  }

  at_utils_conf__pop_mod(conf);

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



void at_mb_iie_et__manifest(const at_mb_iie_et_t *et, at_utils_manifest_t *manifest)
{
  at_mb_iie_et_cache_params__manifest(et->cache_params, manifest);
}


#endif
