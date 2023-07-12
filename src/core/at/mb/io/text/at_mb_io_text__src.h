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

/* text IO routines for at_mb_t */
#ifndef AT_MB_IO_TEXT__SRC_H__
#define AT_MB_IO_TEXT__SRC_H__

#include "at_mb_io_text.h"
#include "../../../utils/at_utils.h"
#include "v3/at_mb_io_text_v3__src.h"



static int at_mb__read_text_low_level(
    at_mb_t *mb,
    at_utils_io_t *io)
{
  if (io->version == 3) {
    return at_mb__read_text_v3_low_level(mb, io);
  } else {
    fprintf(stderr, "Error@at.mb.io.text: failed to read text file [%s] of version %d\n",
        io->fn, io->version);
  }

  return -1;
}



int at_mb__read_text(
    at_mb_t *mb,
    const char *fn,
    int *version)
{
  at_utils_io_t io[1];

  at_utils_io_text__init_read_ex(io, "at.mb.io.text", fn);

  if (version) {
    *version = io->version;
  }

  /* call the low-level read function for members */
  if (at_mb__read_text_low_level(mb, io) != 0) {
    goto ERR;
  }

  at_utils_io_text__finish_read_ex(io);
  return 0;

ERR:
  at_utils_io_text__finish_read_ex(io);
  return -1;
}



static int at_mb__write_text_low_level(
    at_mb_t *mb,
    at_utils_io_t *io)
{
  if (io->version == 3) {
    return at_mb__write_text_v3_low_level(mb, io);
  } else {
    fprintf(stderr, "Error@at.mb.io.text: failed to write text file [%s] of version %d\n",
        io->fn, io->version);
  }

  return -1;
}



int at_mb__write_text_versioned(
    at_mb_t *mb,
    const char *fn,
    int version)
{
  at_utils_io_t io[1];

  at_utils_io_text__init_write_ex(io, "at.mb.io.text", fn, version);

  /* call low level write function for members */
  if (at_mb__write_text_low_level(mb, io) != 0) {
    goto ERR;
  }

  at_utils_io_text__finish_write_ex(io);
  return 0;

ERR:

  at_utils_io_text__finish_write_ex(io);
  return -1;
}



int at_mb__write_text(
    at_mb_t *mb,
    const char *fn)
{
  return at_mb__write_text_versioned(mb, fn, AT_MB__FILE_TEXT_VERSION_LATEST);
}


#endif
