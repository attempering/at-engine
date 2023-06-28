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

#ifndef AT_MB_IO__SRC_H__
#define AT_MB_IO__SRC_H__



/* IO routines for at_mb_t */

#include "at_mb_io.h"

/* implementation dependent headers */
#include "../at_mb_basic.h"

#include "binary/at_mb_io_binary__src.h"
#include "text/at_mb_io_text__src.h"



static void at_mb__after_reading(at_mb_t *mb)
{
  // TODO
  (void) mb;
}


int at_mb__read(at_mb_t *mb)
{
  int ret, version;

  //if (mb->use_binary_file) {
  if (1) {

    ret = at_mb__read_binary(mb, mb->file_binary, &version);

    if (ret != 0) {
      fprintf(stderr, "Error@at.mb: cannot load at_mb_t data from %s\n", mb->file_binary);
      return 1;
    } else {
      fprintf(stderr, "Info@at.mb: loaded previous at_mb_t data, %s version: %d\n",
          mb->file_binary, version);
    }

  } else {

    ret = at_mb__read_text(mb, mb->file_text, &version);

    if (ret != 0) {
      fprintf(stderr, "Error@at.mb: cannot load at_mb_t data from %s\n", mb->file_text);
      return 1;
    } else {
      fprintf(stderr, "Info@at.mb: loaded previous at_mb_t data, %s version: %d\n",
        mb->file_text, version);
    }

  }

  at_mb__after_reading(mb);

  return 0;
}


static void at_mb__before_writing(at_mb_t *mb)
{
  at_mb_accum__normalize(mb->accum);
}


int at_mb__write(at_mb_t *mb, at_bool_t write_text_file)
{
  int ret;

  at_mb__before_writing(mb);

  ret = at_mb__write_binary(mb, mb->file_binary);
  if (ret != 0) {
    return ret;
  }

  if (write_text_file) {
    ret = at_mb__write_text(mb, mb->file_text);
  }

  return ret;
}



#endif
