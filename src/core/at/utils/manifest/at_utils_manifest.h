#ifndef AT_UTILS_MANIFEST_H__
#define AT_UTILS_MANIFEST_H__

#include "at_utils_manifest__def.h"

void at_utils_manifest__cfg_init(at_utils_manifest_t *manifest,
    zcom_cfg_t *cfg, int silent);

int at_utils_manifest__open_file(at_utils_manifest_t *manifest);

void at_utils_manifest__close_file(at_utils_manifest_t *manifest);

void at_utils_manifest__finish(at_utils_manifest_t *manifest);

#endif
