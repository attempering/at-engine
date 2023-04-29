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

#include "at/mb/at_mb__src.h"
#include "at/distr/at_distr__src.h"
#include "at/utils/at_utils__src.h"
#include "at/driver/at_driver__src.h"


int main(void)
{
  at_distr_t distr[1];
  at_mb_t mb[1];
  at_driver_t driver[1];
  zcom_cfg_t *cfg = NULL;
  int silent = 1;
  double boltz = 1.0;

  at_distr__cfg_init(distr, boltz, cfg, silent);
  at_mb__cfg_init(mb, distr, cfg, 1.0, NULL, NULL, silent);
  at_driver__cfg_init(driver, distr, mb, cfg, NULL, NULL, silent);

  at_driver__finish(driver);
  at_mb__finish(mb);
  at_distr__finish(distr);

  return 0;
}
