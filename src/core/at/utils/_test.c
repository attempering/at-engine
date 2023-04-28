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

#include "at_utils__src.h"
#include <stdio.h>

int main(void)
{
  zcom_ssm_t *ssm = zcom_ssm__open();

  char *fn = at_utils__make_output_filename(ssm, "data1", "a.dat");

  FILE *fp = fopen(fn, "w");
  if (fp == NULL) {
    fprintf(stderr, "failed to open file %s\n", fn);
  } else {
    fprintf(fp, "hello\n");
    fclose(fp);
  }

  zcom_ssm__close(ssm);

  return 0;
}