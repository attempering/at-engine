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

#ifndef ZCOM__RNG_MT19937_H__
#define ZCOM__RNG_MT19937_H__

#include "../rng__def.h"



/* Mersenne Twister was developed by Makoto Matsumoto and Takuji Nishimura */
#define ZCOM_RNG_MT19937__N 624
#define ZCOM_RNG_MT19937__M 397
#define ZCOM_RNG_MT19937__UMASK 0x80000000UL /* most significant w-r bits */
#define ZCOM_RNG_MT19937__LMASK 0x7fffffffUL /* least significant r bits */


typedef struct zcom_rng_mt19937_t_ {
  int index; /* index of `arr` */
  uint32_t arr[ZCOM_RNG_MT19937__N];
  int init_loaded;
} zcom_rng_mt19937_t;

void zcom_rng_mt19937__init(zcom_rng_mt19937_t *mt, uint32_t seed);

void zcom_rng_mt19937__finish(zcom_rng_mt19937_t *mt);

zcom_rng_mt19937_t *zcom_rng_mt19937__open(uint32_t seed);

void zcom_rng_mt19937__close(zcom_rng_mt19937_t *mt);


/* return an unsigned random number */
uint32_t zcom_rng_mt19937__rand_uint32(zcom_rng_mt19937_t *mt);

/* return a uniformly distributed random number */
double zcom_rng_mt19937__rand_01(zcom_rng_mt19937_t *mt);

/* return a normally distributed with zero mean and unit variance */
double zcom_rng_mt19937__rand_gauss(zcom_rng_mt19937_t *mt);


/* save the current state to file */
int zcom_rng_mt19937__save(zcom_rng_mt19937_t *mt, const char *fn);

void zcom_rng_mt19937__init_from_seed(zcom_rng_mt19937_t *mt, uint32_t seed);

/* load state from `fn`,
 * or if it fails, use `seed` to initialize mt
 * In the latter case, if `seed == 0` then
 * the integer from time(NULL) will be used as the seed
 * 
 * if force_reload == 0, then loading from file is not invoked
 * if a previous file loading operation is already performed.
 * */
int zcom_rng_mt19937__load_or_init_from_seed(
    zcom_rng_mt19937_t *mt,
    const char *fn, uint32_t seed, int force_reload,
    const char *fn_backup);




typedef zcom_rng_mt19937_t zcom_rng_mt_t;

#define zcom_rng_mt__init(mt, seed) \
    zcom_rng_mt19937__init(mt, seed)

#define zcom_rng_mt__open(seed) \
    zcom_rng_mt19937__open(seed)

#define zcom_rng_mt__close(mt) \
    zcom_rng_mt19937__close(mt)

#define zcom_rng_mt__rand_uint32(mt) \
  zcom_rng_mt19937__rand_uint32(mt)

#define zcom_rng_mt__rand_01(mt) \
  zcom_rng_mt19937__rand_01(mt)

#define zcom_rng_mt__rand_gauss(mt) \
  zcom_rng_mt19937__rand_gauss(mt)

#define zcom_rng_mt__save(mt, fn) \
  zcom_rng_mt19937__save(mt, fn)

#define zcom_rng_mt__init_from_seed(mt, seed) \
  zcom_rng_mt19937__init_from_seed(mt, seed)

#define zcom_rng_mt__load_or_init_from_seed(mt, fn, seed, force_reload, fn_backup) \
  zcom_rng_mt19937__load_or_init_from_seed(mt, fn, seed, force_reload, fn_backup)



#endif
