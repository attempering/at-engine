/* 
 * Copyright (C) 2010-2023  At-engine Developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef AT_UTILS_MANIFEST__SRC_H__
#define AT_UTILS_MANIFEST__SRC_H__

#include "at_utils_manifest.h"

#include "../../../zcom/zcom.h"



void at_utils_manifest__cfg_init(at_utils_manifest_t *manifest,
    zcom_cfg_t *cfg, int silent)
{
  manifest->enabled = 1;

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
