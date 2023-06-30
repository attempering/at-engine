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

#include "at/utils/at_utils__src.h"
#include "zcom/zcom__src.h"




int main(int argc, char **argv)
{
  at_utils_log_t log[1];
  zcom_ssm_t *ssm;

  ssm = zcom_ssm__open();

  at_utils_log__cfg_init(log, NULL, ssm, "atdata", AT__TRUE);

  at_utils_log__push_mod(log, "module1");
  at_utils_log__info(log, "ssm pointer %p\n", ssm);
  at_utils_log__pop_mod(log);

  at_utils_log__info(log, "Good bye!\n");

  at_utils_log__finish(log);

  zcom_ssm__close(ssm);

  return 0;
}