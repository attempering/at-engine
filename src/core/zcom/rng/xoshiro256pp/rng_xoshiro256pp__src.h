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

#ifndef ZCOM__RNG_XOSHIRO256PP__SRC_H__
#define ZCOM__RNG_XOSHIRO256PP__SRC_H__

#include "rng_xoshiro256pp.h"
#include "../rng_utils.h"


/* save the current state to file */
int zcom_rng_xoshiro256pp__save(zcom_rng_xoshiro256pp_t *xsr, const char *fn)
{
  FILE* fp;
  int err = 1;

  if ((fp = fopen(fn, "w")) != NULL) {
    fprintf(fp, "# xoshiro256++\n%" PRIu64 "\n%" PRIu64 "\n%" PRIu64 "\n%" PRIu64 "\n",
        xsr->s[0], xsr->s[1], xsr->s[2], xsr->s[3]);
    fclose(fp);
    err = 0;
  }

  return err;
}


inline static uint64_t zcom_rng_xoshiro256pp__splitmix64_(uint64_t *seed)
{
  uint64_t z = (*seed += 0x9e3779b97f4a7c15ull);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
  return z ^ (z >> 31);
}


void zcom_rng_xoshiro256pp__init_from_seed(zcom_rng_xoshiro256pp_t *xsr, uint64_t seed)
{
  if (seed == 0) {
    seed = (uint64_t) time(NULL);
  }

  xsr->s[0] = zcom_rng_xoshiro256pp__splitmix64_(&seed);
  xsr->s[1] = zcom_rng_xoshiro256pp__splitmix64_(&seed);
  xsr->s[2] = zcom_rng_xoshiro256pp__splitmix64_(&seed);
  xsr->s[3] = zcom_rng_xoshiro256pp__splitmix64_(&seed);
}



int zcom_rng_xoshiro256pp__load(zcom_rng_xoshiro256pp_t *xsr, const char *fn)
{
  FILE* fp;
  int err = 1;

  if ((fp = fopen(fn, "r")) != NULL) {
    char buf[1024];

    /* read the tag line: "# xoshiro256++" */
    if (fgets(buf, sizeof buf, fp)) {
      int i;
      uint64_t s[4];

      err = 0;
      for (i = 0; i < 4; i++) {
        if (fgets(buf, sizeof buf, fp) == NULL) {
          err = 1;
          break;
        }

        if (1 != sscanf(buf, "%" SCNu64, &s[i])) {
          err = 1;
          break;
        }
      }

      if (err == 0) {
        xsr->s[0] = s[0];
        xsr->s[1] = s[1];
        xsr->s[2] = s[2];
        xsr->s[3] = s[3];
      }

    }

    fclose(fp);
  }

  xsr->init_loaded = (err == 0);

  return err;
}



int zcom_rng_xoshiro256pp__load_or_init_from_seed(
    zcom_rng_xoshiro256pp_t *xsr,
    const char *fn,
    uint64_t seed,
    int force_reload,
    const char *fn_backup)
{
  int err;
  
  if (force_reload || !xsr->init_loaded) {
    //fprintf(stderr, "start loading rng from file %s, %d %d\n", fn, force_reload, xsr->init_loaded);
    err = zcom_rng_xoshiro256pp__load(xsr, fn);

    if (err) { /* fallback: to initialize from the seed */

      fprintf(stderr, "Info@zcom.rng.xoshiro256pp: failed to load file \"%s\", "
          "init. from seed: %" PRIu64 " directly, init_loaded %d, force_reload %d.\n",
          fn, seed, xsr->init_loaded, force_reload);

      zcom_rng_xoshiro256pp__init_from_seed(xsr, seed);

    } else {

      fprintf(stderr, "Info@zcom.rng.xoshiro256pp: successfully loaded state file \"%s\", "
          "init_loaded %d, force_reload %d.\n",
          fn, xsr->init_loaded, force_reload);

    }

    if (fn_backup != NULL) {

      fprintf(stderr, "Info@zcom.rng.xoshiro256pp: saved state file \"%s\".\n", fn_backup);

      zcom_rng_xoshiro256pp__save(xsr, fn_backup);

    }


  } else {
    err = 0;
  }


  return err;
}



void zcom_rng_xoshiro256pp__init(zcom_rng_xoshiro256pp_t *xsr, uint64_t seed)
{
  xsr->init_loaded = 0;

  zcom_rng_xoshiro256pp__init_from_seed(xsr, seed);
}



zcom_rng_xoshiro256pp_t *zcom_rng_xoshiro256pp__open(uint64_t seed)
{
  zcom_rng_xoshiro256pp_t *xsr = (zcom_rng_xoshiro256pp_t *) calloc(1, sizeof(zcom_rng_xoshiro256pp_t));
  if (xsr == NULL) {
    exit(1);
  }

  zcom_rng_xoshiro256pp__init(xsr, seed);

  return xsr;
}



void zcom_rng_xoshiro256pp__close(zcom_rng_xoshiro256pp_t *xsr)
{
  free(xsr);
}



inline static uint64_t zcom_rng_xoshiro256pp__rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}


/* return an unsigned random number */
uint64_t zcom_rng_xoshiro256pp__next(zcom_rng_xoshiro256pp_t *xsr)
{
  uint64_t result = zcom_rng_xoshiro256pp__rotl(xsr->s[0] + xsr->s[3], 23) + xsr->s[0];

  uint64_t t = xsr->s[1] << 17;

  xsr->s[2] ^= xsr->s[0];
  xsr->s[3] ^= xsr->s[1];
  xsr->s[1] ^= xsr->s[2];
  xsr->s[0] ^= xsr->s[3];

  xsr->s[2] ^= t;

  xsr->s[3] = zcom_rng_xoshiro256pp__rotl(xsr->s[3], 45);

  return result;
}


/* next() from xoshiro256+ for generating floating-point random numbers
   The state evolution is the same as that for xoshiro256++ */
static uint64_t zcom_rng_xoshiro256pp__xoshiro256p_next(zcom_rng_xoshiro256pp_t *xsr)
{
  uint64_t result = xsr->s[0] + xsr->s[3];

  uint64_t t = xsr->s[1] << 17;

  xsr->s[2] ^= xsr->s[0];
  xsr->s[3] ^= xsr->s[1];
  xsr->s[1] ^= xsr->s[2];
  xsr->s[0] ^= xsr->s[3];

  xsr->s[2] ^= t;

  xsr->s[3] = zcom_rng_xoshiro256pp__rotl(xsr->s[3], 45);

  return result;
}



/* This is the jump function for the generator. It is equivalent
   to 2^128 calls to __next(); it can be used to generate 2^128
   non-overlapping subsequences for parallel computations. */
void zcom_rng_xoshiro256pp__jump(zcom_rng_xoshiro256pp_t *xsr)
{
  static const uint64_t JUMP[] = {
    0x180ec6d33cfd0aba,
    0xd5a61266f0c9392c,
    0xa9582618e03fc9aa,
    0x39abdc4529b1661c
  };

  size_t i;
  int b;
  uint64_t s[4] = {0, 0, 0, 0};
  for (i = 0; i < sizeof JUMP / sizeof *JUMP; i++) {
    for (b = 0; b < 64; b++) {
      if (JUMP[i] & UINT64_C(1) << b) {
        s[0] ^= xsr->s[0];
        s[1] ^= xsr->s[1];
        s[2] ^= xsr->s[2];
        s[3] ^= xsr->s[3];
      }
      zcom_rng_xoshiro256pp__next(xsr);  
    }
  }
    
  xsr->s[0] = s[0];
  xsr->s[1] = s[1];
  xsr->s[2] = s[2];
  xsr->s[3] = s[3];
}


/* This is the long-jump function for the generator. It is equivalent to
   2^192 calls to __next(); it can be used to generate 2^64 starting points,
   from each of which __jump() will generate 2^64 non-overlapping
   subsequences for parallel distributed computations. */
void zcom_rng_xoshiro256pp__long_jump(zcom_rng_xoshiro256pp_t *xsr)
{
  static const uint64_t LONG_JUMP[] = {
    0x76e15d3efefdcbbf,
    0xc5004e441c522fb3,
    0x77710069854ee241,
    0x39109bb02acbe635 };

  size_t i;
  int b;
  uint64_t s[4] = {0, 0, 0, 0};
  for (i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++) {
    for (b = 0; b < 64; b++) {
      if (LONG_JUMP[i] & UINT64_C(1) << b) {
        s[0] ^= xsr->s[0];
        s[1] ^= xsr->s[1];
        s[2] ^= xsr->s[2];
        s[3] ^= xsr->s[3];
      }
      zcom_rng_xoshiro256pp__next(xsr);  
    }
  }
    
  xsr->s[0] = s[0];
  xsr->s[1] = s[1];
  xsr->s[2] = s[2];
  xsr->s[3] = s[3];
}


uint64_t zcom_rng_xoshiro256pp__rand_uint64(zcom_rng_xoshiro256pp_t *xsr)
{
  return zcom_rng_xoshiro256pp__next(xsr);
}


uint32_t zcom_rng_xoshiro256pp__rand_uint32(zcom_rng_xoshiro256pp_t *xsr)
{
  return (uint32_t) (zcom_rng_xoshiro256pp__rand_uint64(xsr) >> 32);
}


double zcom_rng_xoshiro256pp__rand_01(zcom_rng_xoshiro256pp_t *xsr)
{
  //uint64_t x = zcom_rng_xoshiro256pp__rand_uint64(xsr);

  // xoshiro256+ is a slightly faster alternative to xoshiro256++
  // with some vulnerability at the last bit.
  // But for floating-point-number generations,
  // the last 11 bit are discarded, and it is
  // the recommended method of generation. 
  uint64_t x = zcom_rng_xoshiro256pp__xoshiro256p_next(xsr);

  return zcom_rng__uint64_to_01__(x);
}


double zcom_rng_xoshiro256pp__rand_gauss(zcom_rng_xoshiro256pp_t *xsr)
{
  ZCOM_RNG__BUILD_RAND_GAUSS__(xsr, zcom_rng_xoshiro256pp__rand_01)
}



#endif
