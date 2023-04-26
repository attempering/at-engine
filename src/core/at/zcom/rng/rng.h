#ifndef ZCOM__RNG_H__
#define ZCOM__RNG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


#include "../def/def.h"


/* Including <inttypes.h> automatically includes also <stdint.h>  */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
  #include <inttypes.h>
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef unsigned uint32_t;
  typedef unsigned __int64 uint64_t;
#else
  #include <inttypes.h>
#endif

#ifndef PRIu32
  #if (defined(_MSC_VER) && (_MSC_VER >= 1300)) || defined(__BORLANDC__)
    #define PRIu32 "I32u"
  #else
    #define PRIu32 "u"
  #endif
#endif

#ifndef PRIu64
  #if defined(_MSC_VER) || defined(__BORLANDC__)
    #define PRIu64 "I64u"
  #else
    #define PRIu64 "llu"
  #endif
#endif






/* Mersenne Twister was developped by Makoto Matsumoto and Takuji Nishimura */
#define ZCOM_MT_N 624


typedef struct zcom_mtrng_t_ {
  int index; /* index of `arr` */
  uint32_t arr[ZCOM_MT_N];
} zcom_mtrng_t;



/* save the current mt state to file */
ZCOM_INLINE int zcom_mtrng__save(zcom_mtrng_t *mtrng, const char *fname);

ZCOM_INLINE void zcom_mtrng__init_from_seed(zcom_mtrng_t *mtrng, uint32_t seed);

/* load mt state from `fname`
 * The reason that this function is only an internal function
 * is that it does not have a safety fallback mechanism:
 * if the file is corrupted, then the internal state of the RNG
 * will be a mess.
 * This is why we need another wrapper function to recover from
 * the failure case. */
ZCOM_INLINE int zcom_mtrng__load_from_file_(zcom_mtrng_t *mtrng, const char *fname);

/* load mt state from `fname`,
 * or if it fails, use `seed` to initialize mt
 * In the latter case, if `seed == 0` then
 * the integer from time(NULL) will be used as the seed
 * */
ZCOM_INLINE int zcom_mtrng__load_or_init_from_seed(zcom_mtrng_t *mtrng, const char *fname, uint32_t seed);

ZCOM_INLINE zcom_mtrng_t *zcom_mtrng__open(uint32_t seed);

ZCOM_INLINE void zcom_mtrng__close(zcom_mtrng_t *mtrng);


/* return an unsigned random number */
ZCOM_INLINE uint32_t zcom_mtrng__randuint32(zcom_mtrng_t *mtrng);

/* return a uniformly distributed random number */
ZCOM_INLINE double zcom_mtrng__rand01(zcom_mtrng_t *mtrng);

/* return a normally distributed with zero mean and unit variance
 * using ratio method */
ZCOM_INLINE double zcom_mtrng__randgaus(zcom_mtrng_t *mtrng);


#endif
