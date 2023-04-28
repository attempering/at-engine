/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

#ifndef UTILS_SYS__SRC_H__
#define UTILS_SYS__SRC_H__

#include "at_utils_sys.h"

#include <sys/stat.h>

int at_utils_sys__create_dir(const char* path)
{
  return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH);
}

int at_utils_sys__dir_exists(const char *path)
{
  struct stat info;
  if (stat(path, &info) != 0) {
    return 0;
  }
  return S_ISDIR(info.st_mode);
}


int at_utils_sys__create_dir_if_not_exists(const char *path)
{
  if (!at_utils_sys__dir_exists(path)) {
    return at_utils_sys__create_dir(path);
  }

  return 0;
}

#endif
