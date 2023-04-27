// gcc test.c aux.c
#include "../util.h"

int main(int argc, char **argv)
{
  zcom_util__fatal("fatal %d\n", argc);

  return 0;
}
