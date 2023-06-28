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

#ifndef AT_EH_IO_BINARY__SRC_H__
#define AT_EH_IO_BINARY__SRC_H__

#include "at_eh_io_binary.h"
#include "../../at_eh_basic.h"
#include "../../../../zcom/zcom.h"


static int at_eh__read_binary_low(at_eh_t *eh, at_utils_io_t *io)
{
  int i;
  double *pd;
  int jmin;
  int size;

  if (eh == NULL) {
    fprintf(stderr, "passing null pointer to eh_read_binary_low\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* clear data before reading */
  at_eh__clear(eh);

  /* n: number of temperature bins */
  if (at_utils_io_binary__read_and_compare_int(io, NULL, "eh->n", eh->n) != 0) {
    goto ERR;
  }

  /* eh_cnt: number of energy bins */
  if (at_utils_io_binary__read_and_compare_int(io, NULL, "eh->cnt", eh->cnt) != 0) {
    goto ERR;
  }

  /* eh_min: minimal energy */
  if (at_utils_io_binary__read_and_compare_double(io, NULL, "eh->min", eh->min, 1e-5) != 0) {
    goto ERR;
  }

  /* eh_del: energy bin size */
  if (at_utils_io_binary__read_and_compare_double(io, NULL, "eh->del", eh->del, 1e-5) != 0) {
    goto ERR;
  }

  /* eh_his: energy histogram data */
  for (i = 0; i < eh->n; i++) {

    if (at_utils_io_binary__read_and_compare_int(io, NULL, "eh-index", i) != 0) {
      goto ERR;
    }

    pd = eh->his + i * eh->cnt;

    if (at_utils_io_binary__read_int(io, &jmin, "eh-jmin",
        AT_UTILS_IO__NONNEGATIVE) != 0) {
      goto ERR;
    }

    if ( !(jmin >= 0 && jmin < eh->cnt) ) {
      fprintf(stderr, "eh->his: base index %d out of boundary [0, eh->cnt=%d)\n",
          jmin, eh->cnt);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    if (at_utils_io_binary__read_int(io, &size, "eh-size",
    
        AT_UTILS_IO__NONNEGATIVE) != 0) {
      goto ERR;
    }

    if (size <= 0) {
      continue;
    }

    if (jmin + size > eh->cnt) {
      fprintf(stderr, "Error@at.eh: eh->his: invalid size %d, jmin=%d, [0, eh->cnt=%d)\n",
          size, jmin, eh->cnt);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }


    if (at_utils_io_binary__read_double_array(io, size, pd+jmin, "eh-arr",
        AT_UTILS_IO__NONNEGATIVE) != 0) {
      goto ERR;
    }

  }

  return 0;

ERR:

  at_eh__clear(eh);
  return -1;
}



int at_eh__read_binary(at_eh_t *eh, const char *fn, int *version)
{
  at_utils_io_t io[1];

  if (eh->mode == 0) {
    return 0;
  }

  if ( !(eh->mode == 1) ) {
    fprintf(stderr, "at_eh__read_binary: failed validation: eh->mode == 1\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  if (at_utils_io_binary__init_read_ex(io, "at.eh.io.binary", fn) != 0) {
    return -1;
  }

  if (version != NULL) {
    *version = io->version;
  }

  /* call low level read function for members */
  if (at_eh__read_binary_low(eh, io) != 0) {
    goto ERR;
  }

  at_utils_io_binary__finish_read_ex(io);
  return 0;
ERR:
  at_utils_io_binary__finish_read_ex(io);
  return -1;
}



static int at_eh__write_binary_low(at_eh_t *eh, at_utils_io_t *io)
{
  int i;
  int jmin;
  int jmax;
  double *pd;
  int size;

  if (eh == NULL) {
    fprintf(stderr, "passing null pointer to eh_read_binary_low\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* n: number of temperature bins */
  if (at_utils_io_binary__write_int(io, eh->n, "eh->n") != 0) {
    goto ERR;
  }

  /* eh_cnt: number of energy bins */
  if (at_utils_io_binary__write_int(io, eh->cnt, "eh->cnt") != 0) {
    goto ERR;
  }

  /* eh_min: minimal energy */
  if (at_utils_io_binary__write_double(io, eh->min, "eh_min") != 0) {
    goto ERR;
  }

  /* eh_del: energy bin size */
  if (at_utils_io_binary__write_double(io, eh->del, "eh_del") != 0) {
    goto ERR;
  }

  /* eh_his: energy histogram data */
  for (i = 0; i < eh->n; i++) {
    pd = eh->his + i * eh->cnt;
    for (jmax = eh->cnt; jmax > 0 && pd[jmax-1] <= 0.0; jmax--)
      ;
    for (jmin = 0; jmin < jmax && pd[jmin] <= 0.0; jmin++)
      ;

    size = jmax - jmin;
    if (size <= 0) {
      size = 0;
      jmin = 0;
    }

    if (at_utils_io_binary__write_int(io, i, "eh-index") != 0) {
      goto ERR;
    }

    if (at_utils_io_binary__write_int(io, jmin, "eh-jmin") != 0) {
      goto ERR;
    }

    if (at_utils_io_binary__write_int(io, size, "eh-size") != 0) {
      goto ERR;
    }

    if (at_utils_io_binary__write_double_array(io, size, pd+jmin, "eh-arr",
        0) != 0) {
      goto ERR;
    }

  }

  return 0;
ERR:
  return -1;
}



int at_eh__write_binary(at_eh_t *eh, const char *fn, int version)
{
  at_utils_io_t io[1];

  if (eh->mode == 0) {
    return 0;
  }

  if ( !(eh->mode == 1) ) {
    fprintf(stderr, "at_eh__write_binary: failed validation: eh->mode == 1\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  at_utils_io_binary__init_write_ex(io, "at.eh.io.binary", fn, version);

  /* call low level write function for members */
  if (at_eh__write_binary_low(eh, io) != 0) {
    goto ERR;
  }

  at_utils_io_binary__finish_write_ex(io);
  return 0;

ERR:
  at_utils_io_binary__finish_write_ex(io);
  return -1;
}


#endif
