#ifndef UTILS__DEF_H__
#define UTILS__DEF_H__

#include "log/at_utils_log__def.h"
#include "manifest/at_utils_manifest__def.h"

typedef struct at_utils_t_
{
  at_utils_log_t log[1];
  at_utils_manifest_t manifest[1];
} at_utils_t;

#endif
