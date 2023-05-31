/* 
 * Copyright (C) 2010-2023  AT-Engine Developers
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



void at_utils_manifest__cfg_init(
    at_utils_manifest_t *manifest,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  manifest->inited = 1;

  manifest->filename = (char *) "manifest.dat";
  if (zcom_cfg__get(cfg, &manifest->filename, "manifest-file", "%s") != 0)
  {
    if (verbose) fprintf(stderr, "Info@at.utils.manifest: assuming default utils->manifest->filename = \"manifest.dat\", key: manifest-file\n");
  }

  manifest->filename = at_utils__make_output_filename(ssm, data_dir, manifest->filename);

  manifest->arr_max_items = 3;
  if (zcom_cfg__get(cfg, &manifest->arr_max_items, "manifest-arr-max-items", "%d") != 0)
  {
    if (verbose) fprintf(stderr, "Info@at.utils.manifest: assuming default utils->manifest->arr_max_items = 3, key: manifest-arr-max-items\n");
  }

  manifest->fp = NULL; // do not open it yet
}



FILE *at_utils_manifest__open_file(at_utils_manifest_t *manifest)
{
  if ((manifest->fp = fopen(manifest->filename, "w")) != NULL) {
    return manifest->fp;
  }

  return NULL;
}


void at_utils_manifest__close_file(at_utils_manifest_t *manifest)
{
  if (manifest->inited) {
    //fprintf(stderr, "manifest->fp %p\n", manifest->fp);
    //if (manifest->fp) {fprintf(stderr, "error %d\n", ferror(manifest->fp)); getchar();}

    if (manifest->fp != NULL) {
      fclose(manifest->fp);
      manifest->fp = NULL;
    }
  }
}


void at_utils_manifest__finish(at_utils_manifest_t *manifest)
{
  if (manifest->inited) {
    at_utils_manifest__close_file(manifest);
    manifest->inited = 0;
  }
}


void at_utils_manifest__manifest(at_utils_manifest_t *manifest)
{
  fprintf(manifest->fp, "utils->manifest->filename: char *, %s\n", manifest->filename);
}



void at_utils_manifest__print_int(at_utils_manifest_t *manifest,
    int value, const char *var, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  if (cfg_key != NULL) {
    fprintf(fp, "%s (int %s): %d\n", var, cfg_key, value);
  } else {
    fprintf(fp, "%s (int): %d\n", var, value);
  }
}


void at_utils_manifest__print_double(at_utils_manifest_t *manifest,
    double value, const char *var, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  if (cfg_key != NULL) {
    fprintf(fp, "%s (double %s): %g\n", var, cfg_key, value);
  } else {
    fprintf(fp, "%s (double): %g\n", var, value);
  }
}


void at_utils_manifest__print_str(at_utils_manifest_t *manifest,
    const char *value, const char *var, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  if (cfg_key != NULL) {
    fprintf(fp, "%s (str %s): %s\n", var, cfg_key, value);
  } else {
    fprintf(fp, "%s (str): %s\n", var, value);
  }
}


void at_utils_manifest__print_bool(at_utils_manifest_t *manifest,
    at_bool_t value, const char *var, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  if (cfg_key != NULL) {
    fprintf(fp, "%s (bool %s): %s\n", var, cfg_key, (value ? "true" : "false"));
  } else {
    fprintf(fp, "%s (bool): %s\n", var, (value ? "true" : "false"));
  }
}



void at_utils_manifest__print_int_arr(at_utils_manifest_t *manifest,
    const int *arr, int n, const char *name)
{
  int i, pacnt;
  FILE *fp = manifest->fp;
  int arrmax = manifest->arr_max_items;

  fprintf(fp, "%s: dynamic int array of size %d: ", name, n);

  for (i = n-1; i >= 0; i--) {
    if (arr[i] > 0) break;
  }

  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && n > 6) {
      fprintf(fp, "\n");
    }
    for (pacnt = 0, i = 0; i < n; i++) {
      if (i == arrmax && i < n-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) {
          fprintf(fp, "\n");
        }
        fprintf(fp, "..., ");
        if (arrmax > 3) {
          fprintf(fp, "\n");
        }
      }
      if (arrmax >= 0 && i >= arrmax && i < (n-arrmax)) {
        continue;
      }
      fprintf(fp, "%4d, ", arr[i]);
      if (++pacnt % 10 == 0) {
        fprintf(fp, "\n");
      }
    }
    if (pacnt % 10 != 0) {
      fprintf(fp, "\n");
    }
  } else {
    fprintf(fp, " {0}\n");
  }
}



void at_utils_manifest__print_double_arr(at_utils_manifest_t *manifest,
    const double *arr, int n, const char *name)
{
  int i, pacnt;
  FILE *fp = manifest->fp;
  int arrmax = manifest->arr_max_items;

  fprintf(fp, "%s: dynamic double array of size %d: ", name, n);

  for (i = n-1; i >= 0; i--) {
    if (arr[i] > 0) break;
  }

  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && n > 6) {
      fprintf(fp, "\n");
    }
    for (pacnt = 0, i = 0; i < n; i++) {
      if (i == arrmax && i < n-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) {
          fprintf(fp, "\n");
        }
        fprintf(fp, "..., ");
        if (arrmax > 3) {
          fprintf(fp, "\n");
        }
      }
      if (arrmax >= 0 && i >= arrmax && i < (n-arrmax)) {
        continue;
      }
      fprintf(fp, "%g, ", arr[i]);
      if (++pacnt % 10 == 0) {
        fprintf(fp, "\n");
      }
    }
    if (pacnt % 10 != 0) {
      fprintf(fp, "\n");
    }
  } else {
    fprintf(fp, " {0}\n");
  }
}




#endif
