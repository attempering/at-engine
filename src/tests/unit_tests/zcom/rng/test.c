#include "zcom/zcom__src.h"


int test_mt19937_load(const char *fn)
{
  zcom_rng_mt19937_t mt[1];
  uint32_t seed = 1234;

  zcom_rng_mt19937__init(mt, seed);

  zcom_rng_mt19937__load_or_init_from_seed(mt, fn, seed, 0, "mt-init.dat");

  return 0;
}


int test_splitmix64_load(const char *fn)
{
  zcom_rng_splitmix64_t sm[1];
  uint64_t seed = (uint64_t) time(NULL);

  zcom_rng_splitmix64__init(sm, seed);

  zcom_rng_splitmix64__load_or_init_from_seed(sm, fn, seed, 0, "sm-init.dat");

  return 0;
}


int test_xoshiro256pp_load(const char *fn)
{
  zcom_rng_xoshiro256pp_t xsr[1];
  uint64_t seed = (uint64_t) time(NULL);

  zcom_rng_xoshiro256pp__init(xsr, seed);

  zcom_rng_xoshiro256pp__load_or_init_from_seed(xsr, fn, seed, 0, "xsr-init.dat");

  return 0;
}



int main(void)
{
  test_mt19937_load("mt.dat");

  test_splitmix64_load("sm.dat");

  test_xoshiro256pp_load("xsr.dat");

  return 0;
}