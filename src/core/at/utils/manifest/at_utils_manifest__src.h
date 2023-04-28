#ifndef AT_UTILS_MANIFEST__SRC_H__
#define AT_UTILS_MANIFEST__SRC_H__

#include "at_utils_manifest.h"

#include "../../../zcom/zcom.h"



void at_utils_manifest__cfg_init(at_utils_manifest_t *manifest,
    zcom_cfg_t *cfg, int silent)
{
  manifest->filename = "at-manifest.dat";
  if (cfg != NULL && zcom_cfg__get(cfg, &manifest->filename, "manifest_file", "%s"))
  {
    fprintf(stderr, "assuming default: manifest->filename = \"at-manifest.dat\", key: manifest_file\n");
  }

  manifest->arr_max_items = 3;
  if (cfg != NULL && zcom_cfg__get(cfg, &manifest->arr_max_items, "manifest_arr_max_items", "%d"))
  {
    fprintf(stderr, "assuming default: manifest->arr_max_items = 3, key: manifest_arr_max_items\n");
  }

}



int at_utils_manifest__open_file(at_utils_manifest_t *manifest)
{
  if ((manifest->fp = fopen(manifest->filename, "w")) == NULL) {
    return -1;
  }
  return 0;
}


void at_utils_manifest__close_file(at_utils_manifest_t *manifest)
{
  if (manifest->fp != NULL) {
    fclose(manifest->fp);
    manifest->fp = NULL;
  }
}


void at_utils_manifest__finish(at_utils_manifest_t *manifest)
{
  at_utils_manifest__close_file(manifest);
}


#endif
