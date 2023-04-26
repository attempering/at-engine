#include "gmx_fatal.h"
#include <stdio.h>
#include <stdlib.h>

FILE    *debug;

void gmx_fatal(int fatal_errno, const char *file, int line, const char *fmt, ...)
{
  exit(1);
}

