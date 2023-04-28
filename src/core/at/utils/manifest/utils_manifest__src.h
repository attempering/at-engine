#ifndef AT__UTILS_MANIFEST__SRC_H__
#define AT__UTILS_MANIFEST__SRC_H__

#include "utils_manifest.h"

#include "../../../zcom/zcom.h"

void at_utils_manifest__cfg_init(at_utils_manifest_t *manifest,
    zcom_cfg_t *cfg, int silent)
{

}

void at_utils_manifest__finish(at_utils_manifest_t *manifest)
{
  fclose(manifest->fp);
}


#endif
