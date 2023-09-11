#include "zcom/zcom__src.h"


static void test_fatal(void)
{
  int err_num = 1;
  zcom_utils__fatal("fatal %d\n", err_num);
}



int main(void)
{
  test_fatal();

  return 0;
}