#include "zcom/zcom__src.h"

int test_load(const char *fn)
{
  zcom_mtrng_t mtrng[1];
  uint32_t seed = 1234;

  zcom_mtrng__init_from_seed(mtrng, seed);

  zcom_mtrng__load_or_init_from_seed(mtrng, fn, seed, 0, "rng-init.dat");

  return 0;
}


int main(void)
{
  test_load("rng.dat");

  return 0;
}