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

#include "at/at__src.h"
#include "zcom/zcom__src.h"


static void test_conf(at_utils_conf_t *conf)
{
  double double_value = 0.0;
  int int_value = 0;

  at_utils_conf__push_mod(conf, "test.module");

  at_utils_conf__get_double(conf,
      "double-1,dbl1",
      &double_value, 3.14, "double-value-1");

  at_utils_conf__get_int(conf,
      "int1,integer-1",
      &int_value, 314, "int-value-1");

  at_utils_conf__pop_mod(conf);

  printf("double value: %g\n", double_value);
  printf("int value: %d\n", int_value);
}


int main(void)
{
  at_t at[1];
  const char *fn_cfg = "at.cfg";

  if (at__init(at, fn_cfg, NULL, AT__INIT_VERBOSE) != 0) {
    return -1;
  }

  test_conf(at->utils->conf);

  at__finish(at);

  return 0;
}