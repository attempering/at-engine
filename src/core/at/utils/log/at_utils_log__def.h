#ifndef AT_UTILS_LOG__DEF_H__
#define AT_UTILS_LOG__DEF_H__

#include <stdio.h>

typedef struct at_utils_log_t_
{
  char *filename;
  FILE *fp;
} at_utils_log_t;

#endif
