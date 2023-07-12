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

/* binary IO routines for at_mb_t */
#ifndef AT_MB_IO_BINARY__SRC_H__
#define AT_MB_IO_BINARY__SRC_H__

#include "at_mb_io_binary.h"

/* implementation headers */
#include "../../at_mb_basic.h"
#include "../../sm/at_mb_sm.h"
#include "../../accum/at_mb_accum.h"
#include "../../../distr/at_distr.h"

#include "v2/at_mb_io_binary_v2__src.h"
#include "v3/at_mb_io_binary_v3__src.h"



static int at_mb__read_binary_low_level(
    at_mb_t *mb,
    at_utils_io_t *io)
{
  if (io->version == 2) {
    return at_mb__read_binary_v2_low_level(mb, io);
  } else if (io->version == 3) {
    return at_mb__read_binary_v3_low_level(mb, io);
  } else {
    fprintf(stderr, "Error@at.mb.io.binary: failed to read binary file [%s] of version %d\n",
        io->fn, io->version);
  }

  return -1;
}



int at_mb__read_binary(
    at_mb_t *mb,
    const char *fn,
    int *version)
{
  at_utils_io_t io[1];

  if (at_utils_io_binary__init_read_ex(io, "at.mb.io.binary", fn) != 0) {
    return -1;
  }

  if (version != NULL) {
    *version = io->version;
  }

  /* call the low-level read function for members */
  if (at_mb__read_binary_low_level(mb, io) != 0) {
    goto ERR;
  }

  at_utils_io_binary__finish_read_ex(io);
  return 0;

ERR:
  at_utils_io_binary__finish_read_ex(io);
  return -1;
}



static int at_mb__write_binary_low_level(
    at_mb_t *mb,
    at_utils_io_t *io)
{
  if (io->version == 2) {
    return at_mb__write_binary_v2_low_level(mb, io);
  } else if (io->version == 3) {
    return at_mb__write_binary_v3_low_level(mb, io);
  } else {
    fprintf(stderr, "Error@%s: failed to write binary file [%s] of version %d\n",
        io->module, io->fn, io->version);
  }

  return -1;
}



/*
 * For version 2 or lower, make sure to normalize the accumulators
 * before writing.
 **/
int at_mb__write_binary_versioned(
    at_mb_t *mb,
    const char *fn,
    int version)
{
  at_utils_io_t io[1];

  at_utils_io_binary__init_write_ex(io, "at.mb.io.binary", fn, version);

  /* call low level write function for members */
  if (at_mb__write_binary_low_level(mb, io) != 0) {
    goto ERR;
  }

  at_utils_io_binary__finish_write_ex(io);
  return 0;
ERR:
  at_utils_io_binary__finish_write_ex(io);
  return -1;
}



int at_mb__write_binary(
    at_mb_t *mb,
    const char *fn)
{
  return at_mb__write_binary_versioned(mb, fn, AT_MB__FILE_BINARY_VERSION_LATEST);
}



#endif
