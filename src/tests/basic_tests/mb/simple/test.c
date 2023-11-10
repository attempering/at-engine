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

#include "at/context/at_context__def.h"
#include "at/mb/at_mb__src.h"
#include "at/distr/at_distr__src.h"
#include "at/utils/at_utils__src.h"
#include "at/driver/langevin/at_driver_langevin__src.h"
#include "zcom/zcom__src.h"


int main(void)
{
  at_utils_conf_t conf[1];
  at_distr_t distr[1];
  at_mb_t mb[1];
  at_bool_t verbose = AT__FALSE;

  at_utils_conf__init_ez(conf, "at.cfg", "atdata", verbose);
  at_distr__conf_init_ez_(distr, conf);
  at_mb__conf_init_ez_(mb, distr, conf);

  at_mb__finish(mb);
  at_distr__finish(distr);
  at_utils_conf__finish_ez(conf);

  return 0;
}
