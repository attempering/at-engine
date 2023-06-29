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

/* text IO routines for at_mb_t */
#ifndef AT_MB_IO_TEXT__SRC_H__
#define AT_MB_IO_TEXT__SRC_H__

#include "at_mb_io_text.h"

/* implementation headers */
#include "../../at_mb_basic.h"
#include "../../sm/at_mb_sm.h"
#include "../../accum/at_mb_accum.h"
#include "../../../distr/at_distr.h"

#include "../../../../zcom/zcom.h"

#include "v3/at_mb_io_text_v3__src.h"



static int at_mb__read_text_low_level(
    at_mb_t *mb,
    const char* fn,
    FILE *fp,
    int version)
{
  if (version == 3) {
    return at_mb__read_text_v3_low_level(mb, fn, fp);
  } else {
    fprintf(stderr, "Error@at.mb.io: failed to read text file [%s] of version %d\n",
        fn, version);
  }

  return -1;
}



int at_mb__read_text(
    at_mb_t *mb,
    const char *fn,
    int *pver)
{
  FILE *fp;
  int ver;
  int i;

  if ((fp = fopen(fn, "r")) == NULL) {
    fprintf(stderr, "Error@at.mb.io: failed to read text file [%s].\n", fn);
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  if (fscanf(fp, "%d", &ver) != 1) {
    fprintf(stderr, "Error@at.mb.io: error in reading version from %s\n", fn);
    goto ERR;
  }
  if (pver != NULL) {
    *pver = ver;
  }

  /* call the low-level read function for members */
  i = at_mb__read_text_low_level(mb, fn, fp, ver);

  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}



static int at_mb__write_text_low_level(
    at_mb_t *mb,
    const char *fn,
    FILE *fp,
    int version)
{
  if (version == 3) {
    return at_mb__write_text_v3_low_level(mb, fn, fp);
  } else {
    fprintf(stderr, "Error@at.mb.io: failed to write text file [%s] of version %d\n",
        fn, version);
  }

  return -1;
}



int at_mb__write_text_versioned(
    at_mb_t *mb,
    const char *fn,
    int ver)
{
  FILE *fp;
  int i;

  if ((fp = fopen(fn, "w")) == NULL) {
    fprintf(stderr, "Error@at.mb.io: failed to write text file [%s].\n", fn);
    return -1;
  }

  fprintf(fp, "%d\n", ver);

  /* call low level write function for members */
  i = at_mb__write_text_low_level(mb, fn, fp, ver);

  fclose(fp);
  return i;
}


#endif
