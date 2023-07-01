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

#ifndef AT_UTILS_MISC__SRC_H__
#define AT_UTILS_MISC__SRC_H__



#include "sys/at_utils_sys.h"
#include "at_utils_misc.h"



const char *at_utils__bool_to_str(at_bool_t val)
{
  return val ? "true" : "false";
}


char *at_utils__make_output_filename(zcom_ssm_t *ssm, const char *data_dir, const char *fn)
{
  char *buf;

  if (data_dir != NULL) {
    at_utils_sys__create_dir_if_not_exists(data_dir);

    /* buf = data_dir + "/" + fn */
    buf = zcom_ssm__dup(ssm, data_dir);
    zcom_ssm__concat(ssm, &buf, "/");
    zcom_ssm__concat(ssm, &buf, fn);

  } else {

    buf = zcom_ssm__dup(ssm, fn);

  }

  return buf;
}



#endif
