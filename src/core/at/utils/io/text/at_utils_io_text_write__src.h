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

#ifndef AT_UTILS_IO_TEXT_WRITE__SRC_H__
#define AT_UTILS_IO_TEXT_WRITE__SRC_H__

#include "at_utils_io_text_write.h"

#include "../../../../zcom/zcom.h"


int at_utils_io_text__init_write(
    at_utils_io_t *io,
    const char *module,
    const char *fn,
    FILE *fp)
{
  io->module = module;
  io->fn = fn;
  io->fp = fp;

  return 0;
}

int at_utils_io_text__init_write_ex(
    at_utils_io_t *io,
    const char *module,
    const char *fn,
    int version)
{
  io->module = module;
  io->fn = fn;
  io->version = version;

  if ((io->fp = fopen(io->fn, "w")) == NULL) {
    fprintf(stderr, "Error@%s: failed to write text file [%s].\n",
        io->module, io->fn);
    return -1;
  }

  if (at_utils_io_text__write_int(io, version, "version") != 0) {
    return -1;
  }

  return 0;
}


void at_utils_io_text__finish_write_ex(at_utils_io_t *io)
{
  if (io->fp) {
    fclose(io->fp);
    io->fp = NULL;
  }
}



int at_utils_io_text__write_int(
    at_utils_io_t *io,
    int val,
    const char *name)
{
  (void) name;

  fprintf(io->fp, "%d ", val);

  return 0;
}

int at_utils_io_text__write_int_array(
    at_utils_io_t *io,
    int n,
    int *arr,
    const char *name,
    unsigned long flags)
{
  int i;

  if (flags & AT_UTILS_IO__ARRAY_COUNT) {
    at_utils_io_text__write_int(io, n, "_array_count_");
  }

  for (i = 0; i < n; i++) {
    at_utils_io_text__write_int(io, arr[i], name);
  }

  fprintf(io->fp, "\n");

  return 0;
}



int at_utils_io_text__write_bool(
    at_utils_io_t *io,
    at_bool_t val,
    const char *name)
{
  return at_utils_io_text__write_int(io, (int) val, name);
}



int at_utils_io_text__write_double(
    at_utils_io_t *io,
    double val,
    const char *name)
{
  (void) name;

  fprintf(io->fp, "%.15e ", val);

  return 0;
}


int at_utils_io_text__write_double_array(
    at_utils_io_t *io,
    int n,
    double *arr,
    const char *name,
    unsigned long flags)
{
  int i;

  if (flags & AT_UTILS_IO__ARRAY_COUNT) {
    if (at_utils_io_text__write_int(io, n, "_array_count_") != 0) {
      return -1;
    }
  }

  for (i = 0; i < n; i++) {
    at_utils_io_text__write_double(io, arr[i], name);
  }

  fprintf(io->fp, "\n");

  return 0;
}


int at_utils_io_text__write_token(
    at_utils_io_t *io,
    const char *token)
{
  fprintf(io->fp, "%s\n", token);

  return 0;
}


#endif
