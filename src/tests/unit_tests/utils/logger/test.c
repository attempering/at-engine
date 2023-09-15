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




int main(void)
{
  at_utils_logger_t logger[1], logger2[1];
  zcom_ssm_t *ssm;

  ssm = zcom_ssm__open();

  at_utils_logger__cfg_init(logger, NULL, ssm, "atdata", AT__TRUE);

  at_utils_logger__push_mod(logger, "module1");
  at_utils_logger__info(logger, "ssm pointer %p\n", ssm);
  at_utils_logger__pop_mod(logger);

  at_utils_logger__info(logger, "Good bye!\n");

  at_utils_logger__init_delegate(logger2, logger, "delegate");
  at_utils_logger__info(logger2, "I'm a delegate logger.\n");
  at_utils_logger__finish(logger2);

  at_utils_logger__finish(logger);

  zcom_ssm__close(ssm);

  return 0;
}