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

#ifndef AT_DRIVER_LANGEVIN_IO__SRC_H__
#define AT_DRIVER_LANGEVIN_IO__SRC_H__

#include "at_driver_langevin_io.h"


static int at_driver_langevin__read_self(at_driver_langevin_t *langevin)
{
  const char *fn = langevin->file;
  FILE *fp = fopen(fn, "r");

  if (fp == NULL) {
    return -1;
  }

  {
    double total = 0.0, rejects = 0.0;

    if (fscanf(fp, "%lf%lf", &total, &rejects) == 2) {
      langevin->total = round(total);
      langevin->rejects = round(rejects);
    }
  }

  fclose(fp);

  return 0;
}



int at_driver_langevin__read(at_driver_langevin_t *langevin)
{
  at_driver_langevin__read_self(langevin);

  //
  // langevin->rng does not need to be loaded, as it already loaded in
  //  at_driver_langevin_rng__cfg_init();
  //

  return 0;
}



static int at_driver_langevin__write_self(at_driver_langevin_t *langevin)
{
  const char *fn = langevin->file;
  FILE *fp = fopen(fn, "w");
  double ar;

  if (fp == NULL) {
    return -1;
  }

  ar = 1.0 - langevin->rejects/langevin->total;
  fprintf(fp, "%.0f %.0f %.8f\n", langevin->total, langevin->rejects, ar);

  fclose(fp);

  return 0;
}



int at_driver_langevin__write(at_driver_langevin_t *langevin)
{
  int ret1, ret2;

  ret1 = at_driver_langevin__write_self(langevin);
  ret2 = at_driver_langevin_rng__save(langevin->rng);

  return ret1 || ret2;
}


#endif
