/* 
 * Copyright (C) 2010-2023  C. Zhang
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

#ifndef ZCOM__RNG__SRC_H__
#define ZCOM__RNG__SRC_H__

#include "rng.h"


/* save the current mt state to file */
ZCOM__INLINE int zcom_mtrng__save(zcom_mtrng_t *mtrng, const char *fname)
{
  FILE *fp;
  int k;

  if (mtrng->index < 0) {
    return 1; /* RNG was never used, so it cannot be saved */
  }

  if ((fp = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "cannot save to %s.\n", fname);
    return 1;
  }
  fprintf(fp, "MTSEED\n%d\n", mtrng->index);
  for (k = 0; k < ZCOM_RNG__MT_N; k++) {
    fprintf(fp, "%"PRIu32"\n", mtrng->arr[k]);
  }
  fclose(fp);

  return 0;
}



ZCOM__INLINE void zcom_mtrng__init_from_seed(zcom_mtrng_t *mtrng, uint32_t seed)
{
  int k;

  if (seed == 0) {
    seed = (uint32_t) time(NULL);
  }

  mtrng->arr[0] = seed & 0xffffffffUL;
  for (k = 1; k < ZCOM_RNG__MT_N; k++) /* the final mask is for 64-bit machines */
    mtrng->arr[k] = (1812433253UL * (mtrng->arr[k-1] ^ (mtrng->arr[k-1]>>30)) + k) & 0xffffffffUL;
  mtrng->index = ZCOM_RNG__MT_N; /* request for update */
}



/* load mt state from `fname`
 * The reason that this function is only an internal function
 * is that it does not have a safety fallback mechanism:
 * if the file is corrupted, then the internal state of the RNG
 * will be a mess.
 * This is why we need another wrapper function to recover from
 * the failure case. */
ZCOM__INLINE int zcom_mtrng__load_from_file_(zcom_mtrng_t *mtrng, const char *fname)
{
  int err = 1;
  FILE *fp;

  if ((fp = fopen(fname, "r")) != NULL) { /* try to load from file */
    char s[64];

    if (fgets(s, sizeof s, fp) == NULL) {
      fprintf(stderr, "%s is empty\n", fname);
    } else if (strncmp(s, "MTSEED", 6) != 0) { /* to check the first line */
      fprintf(stderr, "mtrand: corrupted file.\n");
    } else if (fscanf(fp, "%d", &mtrng->index) != 1) {
      fprintf(stderr, "no index in %s\n", fname);
    } else {
      int k, z;

      if (mtrng->index < 0) {
        mtrng->index = ZCOM_RNG__MT_N; /* request updating */
      }
      for (z = 1, k = 0; k < ZCOM_RNG__MT_N; k++) {
        if (fscanf(fp, "%"PRIu32, &mtrng->arr[k]) != 1) {
          break;
        }
        if (mtrng->arr[k] != 0) {
          z = 0; /* a non-zero number */
        }
      }

      if (k != ZCOM_RNG__MT_N) {
        fprintf(stderr, "%s incomplete %d/%d\n", fname, k, ZCOM_RNG__MT_N);
      } else {
        err = z; /* clear error, if array is nonzero */
      }

    }

    fclose(fp);
  }

  return err;
}



/* load mt state from `fname`,
 * or if it fails, use `seed` to initialize mt
 * In the latter case, if `seed == 0` then
 * the integer from time(NULL) will be used as the seed
 * */
ZCOM__INLINE int zcom_mtrng__load_or_init_from_seed(zcom_mtrng_t *mtrng, const char *fname, uint32_t seed)
{
  int err = zcom_mtrng__load_from_file_(mtrng, fname);

  if (err) { /* fallback: to initialize from the seed */
    fprintf(stderr, "zcom_mtrng__load_or_init_from_seed(): cannot load file: \"%s\". Init. from seed: %d directly.\n", fname, seed);

    zcom_mtrng__init_from_seed(mtrng, seed);
  } else {
    fprintf(stderr, "Successfully loaded \"%s\".\n", fname);
  }

  return (mtrng->index < 0);
}



ZCOM__INLINE zcom_mtrng_t *zcom_mtrng__open(uint32_t seed)
{
  zcom_mtrng_t *mtrng = (zcom_mtrng_t *) calloc(1, sizeof(zcom_mtrng_t));
  if (mtrng == NULL) {
    exit(1);
  }

  mtrng->index = -1;
  zcom_mtrng__init_from_seed(mtrng, seed);

  return mtrng;
}



ZCOM__INLINE void zcom_mtrng__close(zcom_mtrng_t *mtrng)
{
  free(mtrng);
}



/* return an unsigned random number */
ZCOM__INLINE uint32_t zcom_mtrng__randuint32(zcom_mtrng_t *mtrng)
{
  uint32_t x;
  static const uint32_t mag01[2] = {0, 0x9908b0dfUL}; /* MATRIX_A */
  int k;

  if (mtrng->index < 0) {
    zcom_mtrng__init_from_seed(mtrng, 0);
  }

  if (mtrng->index >= ZCOM_RNG__MT_N) { /* generate ZCOM_RNG__MT_N words at one time */
    for (k = 0; k < ZCOM_RNG__MT_N - ZCOM_RNG__MT_M; k++) {
      x = (mtrng->arr[k] & ZCOM_RNG__MT_UMASK) | (mtrng->arr[k+1] & ZCOM_RNG__MT_LMASK);
      mtrng->arr[k] = mtrng->arr[k+ZCOM_RNG__MT_M] ^ (x>>1) ^ mag01[x&1UL];
    }
    for (; k < ZCOM_RNG__MT_N-1; k++) {
      x = (mtrng->arr[k] & ZCOM_RNG__MT_UMASK) | (mtrng->arr[k+1] & ZCOM_RNG__MT_LMASK);
      mtrng->arr[k] = mtrng->arr[k+(ZCOM_RNG__MT_M-ZCOM_RNG__MT_N)] ^ (x>>1) ^ mag01[x&1UL];
    }
    x = (mtrng->arr[ZCOM_RNG__MT_N-1] & ZCOM_RNG__MT_UMASK) | (mtrng->arr[0] & ZCOM_RNG__MT_LMASK);
    mtrng->arr[ZCOM_RNG__MT_N-1] = mtrng->arr[ZCOM_RNG__MT_M-1] ^ (x>>1) ^ mag01[x&1UL];
    mtrng->index = 0;
  }

  x = mtrng->arr[ mtrng->index++ ];
  /* tempering */
  x ^= (x >> 11);
  x ^= (x <<  7) & 0x9d2c5680UL;
  x ^= (x << 15) & 0xefc60000UL;
  x ^= (x >> 18);
  return x;
}



#undef ZCOM_RNG__MT_N
#undef ZCOM_RNG__MT_M
#undef ZCOM_RNG__MT_UMASK
#undef ZCOM_RNG__MT_LMASK

ZCOM__INLINE double zcom_mtrng__rand01(zcom_mtrng_t *mtrng)
{
  return zcom_mtrng__randuint32(mtrng) * (1.0/4294967296.0);
}



/* Gaussian distribution with zero mean and unit variance
 * using ratio method */
ZCOM__INLINE double zcom_mtrng__randgaus(zcom_mtrng_t *mtrng)
{
  double x, y, u, v, q;

  do {
    u = 1 - zcom_mtrng__rand01(mtrng);
    v = 1.7156*(zcom_mtrng__rand01(mtrng) - .5);  /* >= 2*sqrt(2/e) */
    x = u - 0.449871;
    y = fabs(v) + 0.386595;
    q = x*x  + y*(0.196*y - 0.25472*x);
    if (q < 0.27597) break;
  } while (q > 0.27846 || v*v > -4*u*u*log(u));

  return v/u;
}

#undef ZCOM_RNG__MT_M
#undef ZCOM_RNG__MT_UMASK
#undef ZCOM_RNG__MT_LMASK


#endif