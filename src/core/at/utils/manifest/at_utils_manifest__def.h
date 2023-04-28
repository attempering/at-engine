#ifndef AT_UTILS_MANIFEST__DEF_H__
#define AT_UTILS_MANIFEST__DEF_H__

#include <stdio.h>

typedef struct at_utils_manifest_t_
{
  char *filename;
  FILE *fp;
  int arr_max_items;
} at_utils_manifest_t;

#endif
