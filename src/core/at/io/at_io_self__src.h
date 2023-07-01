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

#ifndef AT_IO_SELF__SRC_H___
#define AT_IO_SELF__SRC_H___

#include "at_io_self.h"
#include "../at_move.h"

#include "../utils/at_utils.h"



int at__write_self_binary(
    at_t *at,
    const char *fn)
{
  at_utils_io_t io[1];
  const int version = 1;

  at_utils_io_binary__init_write_ex(io, "at.io", fn, version);

  if (at_utils_io_binary__write_double(io, at->beta, "at->beta") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, at->energy, "at->energy") != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__write_double(io, at->force_scale, "at->force_scale") != 0) {
    goto ERR;
  }

  at_utils_io_binary__finish_write_ex(io);

  return 0;

ERR:

  at_utils_io_binary__finish_write_ex(io);

  return -1;
}



int at__read_self_binary(
    at_t *at,
    const char *fn)
{
  at_utils_io_t io[1];

  at_utils_io_binary__init_read_ex(io, "at.io", fn);

  if (at_utils_io_binary__read_double(io, &at->beta, "at->beta", AT_UTILS_IO__VERBOSE) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_double(io, &at->energy, "at->energy", AT_UTILS_IO__VERBOSE) != 0) {
    goto ERR;
  }

  if (at_utils_io_binary__read_double(io, &at->force_scale, "at->force_scale", AT_UTILS_IO__VERBOSE) != 0) {
    goto ERR;
  }

  at_utils_io_binary__finish_read_ex(io);

  return 0;

ERR:

  at_utils_io_binary__finish_read_ex(io);

  return -1;
}




int at__read_self_text(
    at_t *at,
    const char *fn)
{
  at_utils_io_t io[1];

  at_utils_io_text__init_read_ex(io, "at.io", fn);

  if (at_utils_io_text__read_double(io, &at->beta, "at->beta",
      AT_UTILS_IO__VERBOSE) != 0) {
      goto ERR;
  }

  if (at_utils_io_text__read_double(io, &at->energy, "at->energy",
      AT_UTILS_IO__VERBOSE) != 0) {
      goto ERR;
  }

  at_utils_io_text__finish_read_ex(io);

  return 0;

ERR:

  at_utils_io_text__finish_read_ex(io);

  return -1;
}



int at__write_self_text(
    at_t *at,
    const char *fn)
{
  at_utils_io_t io[1];
  const int version = 1;

  at_utils_io_text__init_write_ex(io, "at.io", fn, version);

  at_utils_io_text__write_double(io, at->beta, "at->beta");

  at_utils_io_text__write_double(io, at->energy, "at->energy");

  at_utils_io_text__write_double(io, at->force_scale, "at->force_scale");

  at_utils_io_text__finish_write_ex(io);

  return 0;
}



int at__read_self(at_t *at)
{
  int ret;

  ret = at__read_self_binary(at, at->file_binary);

  at__update_force_scale(at);

  return ret;
}



int at__write_self(at_t *at)
{
  int ret;

  ret = at__write_self_binary(at, at->file_binary);

  if (at->write_text_file) {
    at__write_self_text(at, at->file_text);
  }

  return ret;
}


#endif
