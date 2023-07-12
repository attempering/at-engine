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

#ifndef ZCOM__RNG_H__
#define ZCOM__RNG_H__

#include "rng__def.h"

/* save the current mt state to file */
int zcom_mtrng__save(zcom_mtrng_t *mtrng, const char *fname);

void zcom_mtrng__init_from_seed(zcom_mtrng_t *mtrng, uint32_t seed);

/* load mt state from `fname`
 * The reason that this function is only an internal function
 * is that it does not have a safety fallback mechanism:
 * if the file is corrupted, then the internal state of the RNG
 * will be a mess.
 * This is why we need another wrapper function to recover from
 * the failure case. */
int zcom_mtrng__load_from_file_(zcom_mtrng_t *mtrng, const char *fname);

/* load mt state from `fname`,
 * or if it fails, use `seed` to initialize mt
 * In the latter case, if `seed == 0` then
 * the integer from time(NULL) will be used as the seed
 * 
 * if force_reload == 0, then loading from file is not invoked
 * if a previous file loading operation is already performed.
 * */
int zcom_mtrng__load_or_init_from_seed(zcom_mtrng_t *mtrng,
    const char *fn, uint32_t seed, int force_reload,
    const char *fn_backup);

zcom_mtrng_t *zcom_mtrng__open(uint32_t seed);

void zcom_mtrng__close(zcom_mtrng_t *mtrng);


/* return an unsigned random number */
uint32_t zcom_mtrng__rand_uint32(zcom_mtrng_t *mtrng);

/* return a uniformly distributed random number */
double zcom_mtrng__rand01(zcom_mtrng_t *mtrng);

/* return a normally distributed with zero mean and unit variance
 * using ratio method */
double zcom_mtrng__rand_gauss(zcom_mtrng_t *mtrng);


#endif
