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

#ifndef AT_UTILS_IO_BINARY_READ__SRC_H__
#define AT_UTILS_IO_BINARY_READ__SRC_H__

#include "at_utils_io_binary_read.h"


int at_utils_io_binary__init_read(
    at_utils_io_t *io,
    const char *module,
    const char *fn,
    FILE *fp,
    int endn)
{
  io->module = module;
  io->fn = fn;
  io->fp = fp;
  io->endn = endn;

  return 0;
}


int at_utils_io_binary__init_read_ex(
    at_utils_io_t *io,
    const char *module,
    const char *fn)
{
  int itmp;

  io->module = module;
  io->fn = fn;

  if ((io->fp = fopen(io->fn, "rb")) == NULL) {
    fprintf(stderr, "Error@%s: failed to read binary file [%s].\n",
        io->module, io->fn);
    return -1;
  }

  /* determine the endianness */
  if ((io->endn = zcom_endn__rmatchi(&itmp, sizeof(int), io->fp)) < 0) {
    fprintf(stderr, "Error@%s: itmp 0x%X failed to match sizeof(int) 0x%X\n",
        io->module, (unsigned) itmp, (unsigned) sizeof(int));
    goto ERR;
  }

  if (at_utils_io_binary__read_and_compare_int(io, NULL, "sizeof(double)", (int) sizeof(double)) != 0) {
    goto ERR;
  }

  io->version = 0;
  if (at_utils_io_binary__read_int(io, &io->version, "version", 0) != 0) {
    goto ERR;
  }

  return 0;

ERR:
  fclose(io->fp);
  io->fp = NULL;
  return -1;
}



void at_utils_io_binary__finish_read_ex(at_utils_io_t *io)
{
  if (io->fp) {
    fclose(io->fp);
  }
}


int at_utils_io_binary__read_int(
    at_utils_io_t *io,
    int *val,
    const char *name,
    unsigned long flags)
{
  int val_tmp = 0;

  if (zcom_endn__fread(&val_tmp, sizeof(int), 1, io->fp, io->endn) != 1) {
    fprintf(stderr, "Error@%s: error in reading %s from %s\n",
        io->module, name, io->fn);
    return -1;
  }


  if (flags & AT_UTILS_IO__NONNEGATIVE) {
    if (val_tmp < 0) {
      fprintf(stderr, "Error@%s: negative value %s: %d from %s\n",
          io->module, name, val_tmp, io->fn);
      return -1;
    }
  }

  if (flags & AT_UTILS_IO__POSITIVE) {
    if (val_tmp <= 0) {
      fprintf(stderr, "Error@%s: not positive value %s: %d from %s\n",
          io->module, name, val_tmp, io->fn);
      return -1;
    }
  }

  if (flags & AT_UTILS_IO__VERBOSE) {
    fprintf(stderr, "Info@%s: setting %s %d => %d\n",
        io->module, name, *val, val_tmp);
  }

  if (val != NULL) {
    *val = val_tmp;
  }

  return 0;
}



int at_utils_io_binary__read_and_compare_int(
    at_utils_io_t *io,
    int *val,
    const char *name,
    int val_ref)
{
  int val_tmp = 0;

  if (at_utils_io_binary__read_int(io, &val_tmp, name, 0) != 0) {
    return -1;
  }

  if (val_tmp != val_ref) {
    fprintf(stderr, "Error@%s: %s mismatch, read %d, expected %d, pos: %s,%#lx\n",
        io->module, name, val_tmp, val_ref, io->fn, (unsigned long) ftell(io->fp));
    return -1;
  }

  if (val != NULL) {
    *val = val_tmp;
  }

  return 0;
}


int at_utils_io_binary__read_bool(
    at_utils_io_t *io,
    at_bool_t *val,
    const char *name,
    unsigned long flags)
{
  return at_utils_io_binary__read_int(io, (int *) val, name, flags);
}


int at_utils_io_binary__read_and_compare_bool(
    at_utils_io_t *io,
    at_bool_t *val,
    const char *name,
    at_bool_t val_ref)
{
  return at_utils_io_binary__read_and_compare_int(io, (int *) val, name, (int) val_ref);
}



int at_utils_io_binary__read_double(
    at_utils_io_t *io,
    double *val,
    const char *name,
    unsigned long flags)
{
  double val_tmp = 0.0;

  if (zcom_endn__fread(&val_tmp, sizeof(double), 1, io->fp, io->endn) != 1) {
    fprintf(stderr, "Error@%s: error in reading %s from %s\n",
        io->module, name, io->fn);
    return -1;
  }

  if (flags & AT_UTILS_IO__NONNEGATIVE) {
    if (val_tmp < 0) {
      fprintf(stderr, "Error@%s: negative value %s: %g from %s\n",
          io->module, name, val_tmp, io->fn);
      return -1;
    }
  }

  if (flags & AT_UTILS_IO__POSITIVE) {
    if (val_tmp <= 0) {
      fprintf(stderr, "Error@%s: not positive value %s: %g from %s\n",
          io->module, name, val_tmp, io->fn);
      return -1;
    }
  }

  if (flags & AT_UTILS_IO__VERBOSE) {
    fprintf(stderr, "Info@%s: setting %s %g => %g\n",
        io->module, name, *val, val_tmp);
  }

  *val = val_tmp;

  return 0;
}


int at_utils_io_binary__read_double_array(
    at_utils_io_t *io,
    int n,
    double *arr,
    const char *name,
    unsigned long flags)
{
  int i;

  if (flags & AT_UTILS_IO__ARRAY_COUNT) {
    if (at_utils_io_binary__read_and_compare_int(io, NULL, "_array_cnt_", n) != 0) {
      return -1;
    }
  }

  for (i = 0; i < n; i++) {
    double item;

    if (at_utils_io_binary__read_double(io, &item, name, flags) != 0) {
      return -1;
    }

    if (arr != NULL) {
      arr[i] = item;
    }
  }

  return 0;
}



int at_utils_io_binary__read_and_compare_double(
    at_utils_io_t *io,
    double *val,
    const char *name,
    double val_ref,
    double tol)
{
  double val_tmp = 0.0;

  if (at_utils_io_binary__read_double(io, &val_tmp, name, 0) != 0) {
    return -1;
  }

  if (fabs(val_tmp - val_ref) > tol) {
    fprintf(stderr, "Error@%s: %s mismatch, read %g, expected %g, pos: %s,%#lx\n",
        io->module, name, val_tmp, val_ref, io->fn, (unsigned long) ftell(io->fp));
    return -1;
  }

  if (val != NULL) {
    *val = val_tmp;
  }

  return 0;
}


#endif
