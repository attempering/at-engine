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

/* include the source code of sub-modules */
#include "at_mb_io_binary__src.h"




int at_mb__read(at_mb_t *mb, at_driver_langevin_t *langevin, double *beta)
{
  int ret, version;

  ret = at_mb__read_binary(mb, langevin, beta, mb->av_file, &version);

  if (ret != 0) {
    fprintf(stderr, "cannot load at_mb_t data from %s\n", mb->av_file);
    return 1;
  }

  fprintf(stderr, "loaded previous at_mb_t data, %s version: %d\n",
    mb->av_file, version);

  return 0;
}



int at_mb__write(at_mb_t *mb, at_driver_langevin_t *langevin, double beta)
{
  return at_mb__write_binary(mb, langevin, beta, mb->av_file, 1);
}



#endif
