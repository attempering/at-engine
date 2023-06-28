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

#ifndef AT_MB_SHK_IO__SRC_H__
#define AT_MB_SHK_IO__SRC_H__


#include "at_mb_shk_io.h"

#include "../../../utils/at_utils.h"
#include "../../../../zcom/zcom.h"



int at_mb_shk__read_binary(
    at_mb_shk_t *shk,
    at_utils_io_t *io)
{
  if (at_utils_io_binary__read_and_compare_int(io, NULL, "shk->mode", shk->mode) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_and_compare_int(io, NULL, "shk->win_adjusted", shk->win_adjusted) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_double(io, &shk->base, "shk->base",
      AT_UTILS_IO__NONNEGATIVE|AT_UTILS_IO__VERBOSE) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_and_compare_double(io, NULL, "shk->init", shk->init, 1e-8) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_and_compare_double(io, NULL, "shk->max", shk->max, 1e-8) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_and_compare_double(io, NULL, "shk->min", shk->min, 1e-8) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_and_compare_double(io, NULL, "shk->stop", shk->stop, 1.0) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_and_compare_double(io, NULL, "shk->amp", shk->amp, 1e-8) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_and_compare_double(io, NULL, "shk->exp", shk->exp, 1e-8) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_double_array(io, shk->n, NULL, "shk->win_mul",
      AT_UTILS_IO__ARRAY_COUNT|AT_UTILS_IO__POSITIVE) != 0) {
    goto ERR;
  }

  return 0;

ERR:

  return -1;
}



int at_mb_shk__write_binary(
    at_mb_shk_t *shk,
    at_utils_io_t *io)
{
  if (at_utils_io_binary__write_int(io, shk->mode, "shk->mode") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_int(io, shk->win_adjusted, "shk->win_adjusted") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, shk->base, "shk->base") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, shk->init, "shk->init") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, shk->max, "shk->max") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, shk->min, "shk->min") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, shk->stop, "shk->stop") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, shk->amp, "shk->amp") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, shk->exp, "shk->exp") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double_array(io, shk->n, shk->win_mul, "shk->win_mul",
      AT_UTILS_IO__ARRAY_COUNT) != 0) {
    goto ERR;
  }

  return 0;
ERR:

  return -1;
}



int at_mb_shk__read_text(at_mb_shk_t *shk,
    const char *fn, FILE *fp, int version)
{
  (void) version;

  {
    char token[32] = "";

    if (fscanf(fp, "%32s", token) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading begin token from %s\n", fn);
      goto ERR;
    }

    if (strcmp(token, "SHK_BEGIN") != 0) {
      fprintf(stderr, "Error@at.mb.shk: bad starting token [%s]\n", token);
      goto ERR;
    }
  }

  {
    int mode = 0;

    if (fscanf(fp, "%d", &mode) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->mode\n");
      goto ERR;
    }

    if (mode != shk->mode) {
      fprintf(stderr, "Error@at.mb.shk: shk->mode mismatch %d (read) vs %d (expected)\n",
          mode, shk->mode);
      goto ERR;
    }
  }

  {
    int win_adjusted = 0;

    if (fscanf(fp, "%d", &win_adjusted) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->win_adjusted\n");
      goto ERR;
    }

    if (win_adjusted != shk->win_adjusted) {
      fprintf(stderr, "Error@at.mb.shk: shk->win_adjusted mismatch %d (read) vs %d (expected)\n",
          win_adjusted, shk->win_adjusted);
      goto ERR;
    }
  }

  {
    double shk_base = 0.0;

    if (fscanf(fp, "%lf", &shk_base) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->base\n");
      goto ERR;
    }

    fprintf(stderr, "Info@at.mb.shk: setting shk->base %g => %g\n", shk->base, shk_base);
    shk->base = shk_base;
  }

  {
    double shk_init = 0.0;

    if (fscanf(fp, "%lf", &shk_init) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->init\n");
      goto ERR;
    }
  }

  {
    double shk_max = 0.0;

    if (fscanf(fp, "%lf", &shk_max) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->max\n");
      goto ERR;
    }
  }

  {
    double shk_min = 0.0;

    if (fscanf(fp, "%lf", &shk_min) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->min\n");
      goto ERR;
    }
  }

  {
    double shk_stop = 0.0;

    if (fscanf(fp, "%lf", &shk_stop) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->stop\n");
      goto ERR;
    }
  }

  {
    double shk_amp = 0.0;

    if (fscanf(fp, "%lf", &shk_amp) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->amp\n");
      goto ERR;
    }
  }

  {
    double shk_exp = 0.0;

    if (fscanf(fp, "%lf", &shk_exp) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->exp\n");
      goto ERR;
    }
  }

  {
    int n, i;

    if (fscanf(fp, "%d", &n) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading shk->n");
      goto ERR;
    }

    if (n != shk->n) {
      fprintf(stderr, "Error@at.mb.shk: n mismatch %d (read) vs %d (expected)\n",
          n, shk->n);
      goto ERR;
    }

    for (i = 0; i < n; i++) {
      double win_mul;
      if (fscanf(fp, "%lf", &win_mul) != 1) {
        fprintf(stderr, "Error@at.mb.shk: failed to read win_mul[%d]\n", i);
        goto ERR;
      }
    }
  }

  {
    char token[32] = "";

    if (fscanf(fp, "%32s", token) != 1) {
      fprintf(stderr, "Error@at.mb.shk: error in reading end token\n");
      goto ERR;
    }

    if (strcmp(token, "SHK_END") != 0) {
      fprintf(stderr, "Error@at.mb.shk: bad end token [%s]\n", token);
      goto ERR;
    }
  }

  return 0;

ERR:

  return -1;
}



int at_mb_shk__write_text(at_mb_shk_t *shk,
    const char *fn, FILE *fp, int version)
{
  int i;

  (void) fn;
  (void) version;

  fprintf(fp, "SHK_BEGIN\n");

  fprintf(fp, "%d %d\n", shk->mode, shk->win_adjusted);

  fprintf(fp, "%.15e\n", shk->base);

  fprintf(fp, "%.16f %.16f %.15e %.15e %.16f %g\n",
      shk->init, shk->max, shk->min, shk->stop, shk->amp, shk->exp);

  fprintf(fp, "%d\n", shk->n);

  for (i = 0; i < shk->n; i++) {
    fprintf(fp, "%g ", shk->win_mul[i]);
  }

  fprintf(fp, "\n");

  fprintf(fp, "SHK_END\n");

  return 0;
}



#endif
