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

#ifndef AT_DRIVER_LANGEVIN_IO_LEGACY__SRC_H__
#define AT_DRIVER_LANGEVIN_IO_LEGACY__SRC_H__

#include "at_driver_langevin_io_legacy.h"


int at_driver_langevin__read_binary_legacy(
    at_driver_langevin_t *langevin,
    const char *fn,
    FILE *fp,
    int endn)
{
  at_utils_io_t io[1];

  at_utils_io_binary__init_read(io, "at.driver.langevin", fn, fp, endn);

  /* rate */
  double rate = 0.0;
  if (at_utils_io_binary__read_double(io, &rate, "langevin.rejection_rate",
      AT_UTILS_IO__NONNEGATIVE)) {
    goto ERR;
  }

  /* total: total number of attempts of using langevin equation */
  if (at_utils_io_binary__read_double(io, &langevin->total, "langevin->total",
      AT_UTILS_IO__VERBOSE | AT_UTILS_IO__NONNEGATIVE)) {
    goto ERR;
  }

  langevin->rejects = round(langevin->total*rate);

  return 0;

ERR:
  return 1;
}



int at_driver_langevin__write_binary_legacy(
    at_driver_langevin_t *langevin,
    const char *fn, FILE *fp)
{
  at_utils_io_t io[1];

  at_utils_io_binary__init_write(io, "at.driver.langevin", fn, fp);

  /* rejection rate */
  {
    double rate = (langevin->total > 1.0) ? (langevin->rejects/langevin->total) : 0.0;
    if (at_utils_io_binary__write_double(io, rate, "langevin.rejection_rate") != 0) {
      goto ERR;
    }
  }

  /* total: total number of attempts of using langevin equation */
  if (at_utils_io_binary__write_double(io, langevin->total, "langevin->total") != 0) {
    goto ERR;
  }

  return 0;

ERR:

  return 1;
}



#endif
