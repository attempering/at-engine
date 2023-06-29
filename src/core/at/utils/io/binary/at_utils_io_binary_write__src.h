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

#ifndef AT_UTILS_IO_BINARY_WRITE__SRC_H__
#define AT_UTILS_IO_BINARY_WRITE__SRC_H__

#include "at_utils_io_binary_write.h"

#include "../../../../zcom/zcom.h"


int at_utils_io_binary__init_write(
    at_utils_io_t *io,
    const char *module,
    const char *fn,
    FILE *fp)
{
  io->module = module;
  io->fn = fn;
  io->fp = fp;
  io->endn = 1;

  return 0;
}

int at_utils_io_binary__init_write_ex(
    at_utils_io_t *io,
    const char *module,
    const char *fn,
    int version)
{
  io->module = module;
  io->fn = fn;
  io->endn = 1;
  io->version = version;

  if ((io->fp = fopen(io->fn, "wb")) == NULL) {
    fprintf(stderr, "Error@%s: failed to write binary file [%s].\n",
        io->module, io->fn);
    return -1;
  }

  if (at_utils_io_binary__write_int(io, (int) sizeof(int), "sizeof(int)") != 0) {
    return -1;
  }

  if (at_utils_io_binary__write_int(io, (int) sizeof(double), "sizeof(double)") != 0) {
    return -1;
  }

  if (at_utils_io_binary__write_int(io, version, "version") != 0) {
    return -1;
  }

  return 0;
}


void at_utils_io_binary__finish_write_ex(at_utils_io_t *io)
{
  if (io->fp) {
    fclose(io->fp);
    io->fp = NULL;
  }
}



int at_utils_io_binary__write_int(
    at_utils_io_t *io,
    int val,
    const char *name)
{
  if (zcom_endn__fwrite(&val, sizeof(int), 1, io->fp, io->endn) != 1) {
    fprintf(stderr, "Error@%s: error in writing %s to %s\n",
        io->module, name, io->fn);
    return -1;
  }

  return 0;
}

int at_utils_io_binary__write_int_array(
    at_utils_io_t *io,
    int n,
    int *arr,
    const char *name,
    unsigned long flags)
{
  if (flags & AT_UTILS_IO__ARRAY_COUNT) {
    at_utils_io_binary__write_int(io, n, "_array_count_");
  }

  if (zcom_endn__fwrite(arr, sizeof(int), n, io->fp, io->endn) != n) {
    fprintf(stderr, "Error@%s: error in writing %s of size %d to %s\n",
        io->module, name, n, io->fn);
    return -1;
  }

  return 0;
}



int at_utils_io_binary__write_bool(
    at_utils_io_t *io,
    at_bool_t val,
    const char *name)
{
  return at_utils_io_binary__write_int(io, (int) val, name);
}



int at_utils_io_binary__write_double(
    at_utils_io_t *io,
    double val,
    const char *name)
{
  if (zcom_endn__fwrite(&val, sizeof(double), 1, io->fp, io->endn) != 1) {
    fprintf(stderr, "Error@%s: error in writing %s to %s\n",
        io->module, name, io->fn);
    return -1;
  }

  return 0;
}


int at_utils_io_binary__write_double_array(
    at_utils_io_t *io,
    int n,
    double *arr,
    const char *name,
    unsigned long flags)
{
  if (flags & AT_UTILS_IO__ARRAY_COUNT) {
    if (at_utils_io_binary__write_int(io, n, "_array_count_") != 0) {
      return -1;
    }
  }

  if (n > 0) {
    if (zcom_endn__fwrite(arr, sizeof(double), n, io->fp, io->endn) != n) {
      fprintf(stderr, "Error@%s: error in writing %s of size %d to %s\n",
          io->module, name, n, io->fn);
      return -1;
    }
  }

  return 0;
}

#endif
