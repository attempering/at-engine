#include "zcom/zcom__src.h"


static void test_add(void)
{
  double x = 0.52,
         y = -0.51999981,
         z = x + y;
  zcom_xdouble_t x_ = zcom_xdouble__from_double(x),
                 y_ = zcom_xdouble__from_double(y),
                 z_ = zcom_xdouble__add(x_, y_);
  double z2 = zcom_xdouble__to_double(z_);

  printf("Add: z %g, %g (%g*2^%d)\n", z, z2, z_.man, z_.exp);
}


static void test_mul(void)
{
  double x = 0.49,
         y = -0.51,
         z = x * y;
  zcom_xdouble_t x_ = zcom_xdouble__from_double(x),
                 y_ = zcom_xdouble__from_double(y),
                 z_ = zcom_xdouble__mul(x_, y_);
  double z2 = zcom_xdouble__to_double(z_);

  printf("Mul: z %g, %g (%g*2^%d)\n", z, z2, z_.man, z_.exp);
}


static void test_exp(void)
{
  double x = -log(2.00001),
         z = exp(x);
  zcom_xdouble_t z_ = zcom_xdouble__exp(x);
  double z2 = zcom_xdouble__to_double(z_);

  printf("Exp: z %g, %g (%g*2^%d)\n", z, z2, z_.man, z_.exp);
}


int main(void)
{
  test_add();
  test_mul();
  test_exp();

  return 0;
}