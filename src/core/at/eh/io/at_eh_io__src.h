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

#ifndef AT_EH_IO__SRC_H__
#define AT_EH_IO__SRC_H__

#include "at_eh_io.h"
#include "../at_eh_basic.h"

/* include the source code of sub-modules */
#include "binary/at_eh_io_binary__src.h"

#include "../../../zcom/zcom.h"

int at_eh__read(at_eh_t *eh)
{
  int ret = 0, version;

  if (eh != NULL && eh->mode != 0) {
    ret = at_eh__read_binary(eh, eh->file, &version);
  }

  return ret;
}



int at_eh__write(at_eh_t *eh)
{
  int ret = 0;
  const int version = 0;

  if (eh != NULL && eh->mode != 0) {
    ret = at_eh__write_binary(eh, eh->file, version);
  }

  return ret;
}



#endif