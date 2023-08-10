/* 
 * Copyright (C) 2006-2023  AT-Engine Developers
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

#ifndef ZCOM__RNG_SPLITMIX64__SRC_H__
#define ZCOM__RNG_SPLITMIX64__SRC_H__

#include "rng_splitmix64.h"
#include "../rng_utils.h"


/* save the current state to file */
int zcom_rng_splitmix64__save(zcom_rng_splitmix64_t *sm, const char *fn)
{
  FILE* fp;
  int err = 1;

  if ((fp = fopen(fn, "w")) != NULL) {
    fprintf(fp, "# splitmix64\n%" PRIu64 "\n",
        sm->s);
    fclose(fp);
    err = 0;
  }

  return err;
}



void zcom_rng_splitmix64__init_from_seed(zcom_rng_splitmix64_t *sm, uint64_t seed)
{
  if (seed == 0) {
    seed = (uint64_t) time(NULL);
  }

  sm->s = seed * 3141592653589793ull + 2718281828459045ull;
}


int zcom_rng_splitmix64__load(zcom_rng_splitmix64_t *sm, const char *fn)
{
  FILE* fp;
  int err = 1;

  if ((fp = fopen(fn, "r")) != NULL) {
    char buf[1024];

    /* read the tag line: "# splitmix64" */
    if (fgets(buf, sizeof buf, fp)) {
      uint64_t s;

      if (fgets(buf, sizeof buf, fp) != NULL) {
        if (1 == sscanf(buf, "%" SCNu64, &s)) {
          sm->s = s;
          err = 0;
        }
      }
    }

    fclose(fp);
  }

  sm->init_loaded = (err == 0);

  return err;
}



int zcom_rng_splitmix64__load_or_init_from_seed(
    zcom_rng_splitmix64_t *sm,
    const char *fn,
    uint64_t seed,
    int force_reload,
    const char *fn_backup)
{
  int err;
  
  if (force_reload || !sm->init_loaded) {
    //fprintf(stderr, "start loading rng from file %s, %d %d\n", fn, force_reload, sm->init_loaded);
    err = zcom_rng_splitmix64__load(sm, fn);

    if (err) { /* fallback: to initialize from the seed */

      fprintf(stderr, "Info@zcom.rng.splitmix64: failed to load file \"%s\", "
          "init. from seed: %" PRIu64 " directly, init_loaded %d, force_reload %d.\n",
          fn, seed, sm->init_loaded, force_reload);

      zcom_rng_splitmix64__init_from_seed(sm, seed);

    } else {

      fprintf(stderr, "Info@zcom.rng.splitmix64: successfully loaded state file \"%s\", "
          "init_loaded %d, force_reload %d.\n",
          fn, sm->init_loaded, force_reload);

    }

    if (fn_backup != NULL) {

      fprintf(stderr, "Info@zcom.rng.splitmix64: saved state file \"%s\".\n", fn_backup);

      zcom_rng_splitmix64__save(sm, fn_backup);

    }


  } else {
    err = 0;
  }


  return err;
}



void zcom_rng_splitmix64__init(zcom_rng_splitmix64_t *sm, uint64_t seed)
{
  sm->init_loaded = 0;

  zcom_rng_splitmix64__init_from_seed(sm, seed);
}



zcom_rng_splitmix64_t *zcom_rng_splitmix64__open(uint64_t seed)
{
  zcom_rng_splitmix64_t *sm = (zcom_rng_splitmix64_t *) calloc(1, sizeof(zcom_rng_splitmix64_t));
  if (sm == NULL) {
    exit(1);
  }

  zcom_rng_splitmix64__init(sm, seed);

  return sm;
}



void zcom_rng_splitmix64__close(zcom_rng_splitmix64_t *sm)
{
  free(sm);
}



/* return an unsigned random number */
uint64_t zcom_rng_splitmix64__rand_uint64(zcom_rng_splitmix64_t *sm)
{
  uint64_t z = (sm->s += 0x9e3779b97f4a7c15ull);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
  return z ^ (z >> 31);
}


uint32_t zcom_rng_splitmix64__rand_uint32(zcom_rng_splitmix64_t *sm)
{
  return (uint32_t) (zcom_rng_splitmix64__rand_uint64(sm) >> 32);
}


double zcom_rng_splitmix64__rand_01(zcom_rng_splitmix64_t *sm)
{
  return zcom_rng__uint64_to_01__(zcom_rng_splitmix64__rand_uint64(sm));
}


double zcom_rng_splitmix64__rand_gauss(zcom_rng_splitmix64_t *sm)
{
  ZCOM_RNG__BUILD_RAND_GAUSS__(sm, zcom_rng_splitmix64__rand_01)
}



#endif
