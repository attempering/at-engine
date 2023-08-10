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

#ifndef ZCOM__RNG_SPLITMIX64_H__
#define ZCOM__RNG_SPLITMIX64_H__

#include "../rng__def.h"


/* The code for splitmix64 PRNG was adapted from 
   https://prng.di.unimi.it/
 */


typedef struct zcom_rng_splitmix64_t_ {
  uint64_t s;
  int init_loaded;
} zcom_rng_splitmix64_t;


void zcom_rng_splitmix64__init(zcom_rng_splitmix64_t *sm, uint64_t seed);

zcom_rng_splitmix64_t *zcom_rng_splitmix64__open(uint64_t seed);

void zcom_rng_splitmix64__close(zcom_rng_splitmix64_t *sm);

uint64_t zcom_rng_splitmix64__rand_uint64(zcom_rng_splitmix64_t *sm);

/* return an unsigned random number */
uint32_t zcom_rng_splitmix64__rand_uint32(zcom_rng_splitmix64_t *sm);

/* return a uniformly distributed random number */
double zcom_rng_splitmix64__rand_01(zcom_rng_splitmix64_t *sm);

/* return a normally distributed with zero mean and unit variance */
double zcom_rng_splitmix64__rand_gauss(zcom_rng_splitmix64_t *sm);


/* save the current state to file */
int zcom_rng_splitmix64__save(zcom_rng_splitmix64_t *sm, const char *fn);

void zcom_rng_splitmix64__init_from_seed(zcom_rng_splitmix64_t *sm, uint64_t seed);

/* load state from `fn`
 * return 0 on success.
 * On failure, the internal state of the RNG is not modified
 */
int zcom_rng_splitmix64__load(zcom_rng_splitmix64_t *sm, const char *fn);

/* load state from `fn`,
 * or if it fails, use `seed` to initialize mt
 * In the latter case, if `seed == 0` then
 * the integer from time(NULL) will be used as the seed
 * 
 * if force_reload == 0, then loading from file is not invoked
 * if a previous file loading operation is already performed.
 * */
int zcom_rng_splitmix64__load_or_init_from_seed(zcom_rng_splitmix64_t *sm,
    const char *fn, uint64_t seed, int force_reload,
    const char *fn_backup);



#endif
