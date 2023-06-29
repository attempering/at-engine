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

#ifndef AT_MB_SHK_IO_BINARY__SRC_H__
#define AT_MB_SHK_IO_BINARY__SRC_H__


#include "at_mb_shk_io_binary.h"

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



#endif
