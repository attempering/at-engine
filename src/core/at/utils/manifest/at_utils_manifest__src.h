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
#include "../conf/at_utils_conf.h"
#include "../modstack/at_utils_modstack.h"


void at_utils_manifest__conf_init(
    at_utils_manifest_t *manifest,
    at_utils_conf_t *conf)
{
  manifest->ready = AT__TRUE;

  at_utils_conf__push_mod(conf, "at.utils.manifest");

  at_utils_conf__get_filename(conf,
      "manifest-file",
      &manifest->file, "manifest.dat",
      "file");

  at_utils_conf__get_int(conf,
      "manifest-arr-max-items",
      &manifest->arr_max_items, 3,
      "arr_max_items");

  at_utils_conf__pop_mod(conf);

  at_utils_modstack__init(manifest->mods);

  manifest->fp = NULL; // do not open it yet
}



FILE *at_utils_manifest__open_file(at_utils_manifest_t *manifest)
{
  if ((manifest->fp = fopen(manifest->file, "w")) != NULL) {
    return manifest->fp;
  }

  return NULL;
}


void at_utils_manifest__close_file(at_utils_manifest_t *manifest)
{
  if (manifest->ready) {
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
  if (manifest->ready) {
    at_utils_manifest__close_file(manifest);
    at_utils_modstack__finish(manifest->mods);
    manifest->ready = AT__FALSE;
  }
}



void at_utils_manifest__push_mod(at_utils_manifest_t *manifest, const char *mod)
{
  at_utils_modstack__push(manifest->mods, mod);
}


const char *at_utils_manifest__pop_mod(at_utils_manifest_t *manifest)
{
  return at_utils_modstack__pop(manifest->mods);
}


const char *at_utils_manifest__get_mod(at_utils_manifest_t *manifest)
{
  return at_utils_modstack__get(manifest->mods);
}



void at_utils_manifest__manifest(at_utils_manifest_t *manifest)
{
  fprintf(manifest->fp, "at.utils.manifest.file: char *, %s\n", manifest->file);
}



static void at_utils_manifest__print_var_name(at_utils_manifest_t *manifest,
    const char *name, const char *type, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  const char *module = at_utils_manifest__get_mod(manifest); 
  
  if (module != NULL) {
    fprintf(fp, "%s.", module);
  }

  fprintf(fp, "%s ", name);

  if (cfg_key != NULL) {
    fprintf(fp, "(%s %s): ", type, cfg_key);
  } else {
    fprintf(fp, "(%s): ", type);
  }
}



void at_utils_manifest__print_int(at_utils_manifest_t *manifest,
    int value, const char *name, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  at_utils_manifest__print_var_name(manifest, name, "int", cfg_key);

  fprintf(fp, "%d\n", value);
}


void at_utils_manifest__print_double(at_utils_manifest_t *manifest,
    double value, const char *name, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  at_utils_manifest__print_var_name(manifest, name, "double", cfg_key);

  fprintf(fp, "%g\n", value);
}


void at_utils_manifest__print_str(at_utils_manifest_t *manifest,
    const char *value, const char *name, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  at_utils_manifest__print_var_name(manifest, name, "str", cfg_key);

  fprintf(fp, "%s\n", value);
}


void at_utils_manifest__print_bool(at_utils_manifest_t *manifest,
    at_bool_t value, const char *name, const char *cfg_key)
{
  FILE *fp = manifest->fp;

  at_utils_manifest__print_var_name(manifest, name, "bool", cfg_key);

  fprintf(fp, "%s\n", at_utils__bool_to_str(value));
}



void at_utils_manifest__print_int_arr(at_utils_manifest_t *manifest,
    const int *arr, int n, const char *name)
{
  int i, pacnt;
  FILE *fp = manifest->fp;
  int arr_max = manifest->arr_max_items;

  zcom_utils__exit_if (arr == NULL,
    "at.utils.manifest: arr %s %d is NULL\n", name, n);

  at_utils_manifest__print_var_name(manifest, name, "int *", NULL);

  fprintf(fp, "[%d] ", n);

  for (i = n-1; i >= 0; i--) {
    if (arr[i] > 0) break;
  }

  if (i >= 0) {
    if ((arr_max < 0 || arr_max > 3) && n > 6) {
      fprintf(fp, "\n");
    }
    for (pacnt = 0, i = 0; i < n; i++) {
      if (i == arr_max && i < n-arr_max) {
        if (arr_max > 3 && pacnt % 10 != 0) {
          fprintf(fp, "\n");
        }
        fprintf(fp, "..., ");
        if (arr_max > 3) {
          fprintf(fp, "\n");
        }
      }
      if (arr_max >= 0 && i >= arr_max && i < (n-arr_max)) {
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
  int arr_max = manifest->arr_max_items;

  zcom_utils__exit_if (arr == NULL,
    "at.utils.manifest: arr %s %d is NULL\n", name, n);

  at_utils_manifest__print_var_name(manifest, name, "double *", NULL);

  fprintf(fp, "[%d] ", n);

  for (i = n-1; i >= 0; i--) {
    if (arr[i] > 0) {
      break;
    }
  }

  if (i >= 0) {
    if ((arr_max < 0 || arr_max > 3) && n > 6) {
      fprintf(fp, "\n");
    }
    for (pacnt = 0, i = 0; i < n; i++) {
      if (i == arr_max && i < n-arr_max) {
        if (arr_max > 3 && pacnt % 10 != 0) {
          fprintf(fp, "\n");
        }
        fprintf(fp, "..., ");
        if (arr_max > 3) {
          fprintf(fp, "\n");
        }
      }
      if (arr_max >= 0 && i >= arr_max && i < (n-arr_max)) {
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
