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

#ifndef ZCOM__RNG_MT19937__SRC_H__
#define ZCOM__RNG_MT19937__SRC_H__



#include "rng_mt19937.h"
#include "../rng_utils.h"



/* save the current state to file */
int zcom_rng_mt19937__save(zcom_rng_mt19937_t *mt, const char *fn)
{
  FILE *fp;
  int k;

  if (mt->index < 0) {
    return 1; /* RNG was never used, so it cannot be saved */
  }

  if ((fp = fopen(fn, "w")) == NULL) {
    fprintf(stderr, "Error@zcom.rng.mt19937: failed to write %s.\n", fn);
    return 1;
  }

  fprintf(fp, "MTSEED\n%d\n", mt->index);
  for (k = 0; k < ZCOM_RNG_MT19937__N; k++) {
    fprintf(fp, "%" PRIu32 "\n", mt->arr[k]);
  }

  fclose(fp);

  return 0;
}



void zcom_rng_mt19937__init_from_seed(zcom_rng_mt19937_t *mt, uint32_t seed)
{
  int k;

  if (seed == 0) {
    seed = (uint32_t) time(NULL);
  }

  mt->arr[0] = seed & 0xffffffffUL;

  for (k = 1; k < ZCOM_RNG_MT19937__N; k++) { /* the final mask is for 64-bit machines */
    mt->arr[k] = (1812433253UL * (mt->arr[k-1] ^ (mt->arr[k-1]>>30)) + k) & 0xffffffffUL;
  }

  mt->index = ZCOM_RNG_MT19937__N; /* request for update */
}



static int zcom_rng_mt19937__load_from_file_(zcom_rng_mt19937_t *mt, const char *fn)
{
  int err = 1;
  FILE *fp;

  if ((fp = fopen(fn, "r")) != NULL) { /* try to load from file */
    char s[64];

    if (fgets(s, sizeof s, fp) == NULL) {
      fprintf(stderr, "%s is empty\n", fn);
    } else if (strncmp(s, "MTSEED", 6) != 0) { /* to check the first line */
      fprintf(stderr, "mtrand: corrupted file.\n");
    } else if (fscanf(fp, "%d", &mt->index) != 1) {
      fprintf(stderr, "no index in %s\n", fn);
    } else {
      int k, all_zeros;

      if (mt->index < 0) {
        mt->index = ZCOM_RNG_MT19937__N; /* request updating */
      }

      for (all_zeros = 1, k = 0; k < ZCOM_RNG_MT19937__N; k++) {
        if (fscanf(fp, "%" PRIu32, &mt->arr[k]) != 1) {
          break;
        }
        if (mt->arr[k] != 0) {
          all_zeros = 0; /* a non-zero number has been found */
        }
      }

      if (k != ZCOM_RNG_MT19937__N) {
        fprintf(stderr, "Warning@zcom.rng.mt19937: %s incomplete %d/%d\n", fn, k, ZCOM_RNG_MT19937__N);
      } else {
        /* if we are here, it means that all items
           have been read successfully. 
           If, further, one of the array items is nonzero,
           the variable `all_zeros` is false or 0,
           and the error flag can be cleared */
        err = all_zeros;
      }

    }

    fclose(fp);
  }

  mt->init_loaded = (err == 0);

  return err;
}



int zcom_rng_mt19937__load_or_init_from_seed(
    zcom_rng_mt19937_t *mt,
    const char *fn, uint32_t seed, int force_reload,
    const char *fn_backup)
{
  int err;
  
  if (force_reload || !mt->init_loaded) {
    //fprintf(stderr, "start loading rng from file %s, %d %d\n", fn, force_reload, mt->init_loaded);
    err = zcom_rng_mt19937__load_from_file_(mt, fn);

    if (err) { /* fallback: to initialize from the seed */

      fprintf(stderr, "Info@zcom.rng.mt19937: failed to load file \"%s\", "
          "init. from seed: %d directly, init_loaded %d, force_reload %d.\n",
          fn, seed, mt->init_loaded, force_reload);

      zcom_rng_mt19937__init_from_seed(mt, seed);

    } else {

      fprintf(stderr, "Info@zcom.rng.mt19937: successfully loaded state file \"%s\" "
          "(seed %d unused), init_loaded %d, force_reload %d.\n",
          fn, seed, mt->init_loaded, force_reload);

    }

    if (fn_backup != NULL) {

      fprintf(stderr, "Info@zcom.rng.mt19937: saved state file \"%s\".\n", fn_backup);

      zcom_rng_mt19937__save(mt, fn_backup);

    }


  } else {
    err = 0;
  }



  return (mt->index < 0);
}



void zcom_rng_mt19937__init(zcom_rng_mt19937_t *mt, uint32_t seed)
{
  mt->index = -1;
  mt->init_loaded = 0;

  zcom_rng_mt19937__init_from_seed(mt, seed);
}


void zcom_rng_mt19937__finish(zcom_rng_mt19937_t *mt)
{
  (void) mt;
}



zcom_rng_mt19937_t *zcom_rng_mt19937__open(uint32_t seed)
{
  zcom_rng_mt19937_t *mt = (zcom_rng_mt19937_t *) calloc(1, sizeof(zcom_rng_mt19937_t));
  if (mt == NULL) {
    exit(1);
  }

  zcom_rng_mt19937__init(mt, seed);

  return mt;
}



void zcom_rng_mt19937__close(zcom_rng_mt19937_t *mt)
{
  zcom_rng_mt19937__finish(mt);
  free(mt);
}



/* return an unsigned random number */
uint32_t zcom_rng_mt19937__rand_uint32(zcom_rng_mt19937_t *mt)
{
  uint32_t x;
  static const uint32_t mag01[2] = {0, 0x9908b0dfUL}; /* MATRIX_A */
  int k;

  if (mt->index < 0) {
    zcom_rng_mt19937__init_from_seed(mt, 0);
  }

  if (mt->index >= ZCOM_RNG_MT19937__N) { /* generate ZCOM_RNG_MT19937__N words at one time */
    for (k = 0; k < ZCOM_RNG_MT19937__N - ZCOM_RNG_MT19937__M; k++) {
      x = (mt->arr[k] & ZCOM_RNG_MT19937__UMASK) | (mt->arr[k+1] & ZCOM_RNG_MT19937__LMASK);
      mt->arr[k] = mt->arr[k+ZCOM_RNG_MT19937__M] ^ (x>>1) ^ mag01[x&1UL];
    }
    for (; k < ZCOM_RNG_MT19937__N-1; k++) {
      x = (mt->arr[k] & ZCOM_RNG_MT19937__UMASK) | (mt->arr[k+1] & ZCOM_RNG_MT19937__LMASK);
      mt->arr[k] = mt->arr[k+(ZCOM_RNG_MT19937__M-ZCOM_RNG_MT19937__N)] ^ (x>>1) ^ mag01[x&1UL];
    }
    x = (mt->arr[ZCOM_RNG_MT19937__N-1] & ZCOM_RNG_MT19937__UMASK) | (mt->arr[0] & ZCOM_RNG_MT19937__LMASK);
    mt->arr[ZCOM_RNG_MT19937__N-1] = mt->arr[ZCOM_RNG_MT19937__M-1] ^ (x>>1) ^ mag01[x&1UL];
    mt->index = 0;
  }

  x = mt->arr[ mt->index++ ];
  /* tempering */
  x ^= (x >> 11);
  x ^= (x <<  7) & 0x9d2c5680UL;
  x ^= (x << 15) & 0xefc60000UL;
  x ^= (x >> 18);
  return x;
}


double zcom_rng_mt19937__rand_01(zcom_rng_mt19937_t *mt)
{
  return zcom_rng__uint32_to_01__(zcom_rng_mt19937__rand_uint32(mt));
}

double zcom_rng_mt19937__rand_gauss(zcom_rng_mt19937_t *mt)
{
  ZCOM_RNG__BUILD_RAND_GAUSS__(mt, zcom_rng_mt19937__rand_01)
}


#endif
