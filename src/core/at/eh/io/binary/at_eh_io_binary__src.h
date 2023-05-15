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

#ifndef AT_EH_IO_BINARY__SRC_H__
#define AT_EH_IO_BINARY__SRC_H__

#include "at_eh_io_binary.h"
#include "../../at_eh_basic.h"
#include "../../../../zcom/zcom.h"


static int eh_read_binary_low(at_eh_t *eh, FILE *fp, int ver, int endn)
{
  int i;
  int itmp;
  double *pd;
  int jmin;
  double dtmp;
  int size;

  if (eh == NULL) {
    fprintf(stderr, "passing null pointer to eh_read_binary_low\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }
  /* clear data before reading */
  at_eh__clear(eh);

  /* n: number of temperature bins */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != eh->n) {
    fprintf(stderr, "eh->n mismatch, expect: %d, read: %d, pos: %#lx\n",
        eh->n, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* eh_cnt: number of energy bins */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != eh->cnt) {
    fprintf(stderr, "eh->cnt mismatch, expect: %d, read: %d, pos: %#lx\n",
        eh->cnt, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* eh_min: minimal energy */
  if (zcom_endn__fread(&dtmp, sizeof(dtmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading dtmp\n");
    goto ERR;
  }
  if (fabs(dtmp - eh->min) > 1e-5) {
    fprintf(stderr, "eh->min mismatch, expect: %g, read: %g, pos: %#lx\n",
        eh->min, dtmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* eh_del: energy bin size */
  if (zcom_endn__fread(&dtmp, sizeof(dtmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading dtmp\n");
    goto ERR;
  }
  if (fabs(dtmp - eh->del) > 1e-5) {
    fprintf(stderr, "eh->del mismatch, expect: %g, read: %g, pos: %#lx\n",
        eh->del, dtmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if ( !(eh->del > 0) ) {
    fprintf(stderr, "eh->del: failed validation: eh->del > 0\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* eh_his: energy histogram data */
  for (i = 0; i < eh->n; i++) {
    if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading itmp\n");
      if (feof(fp)) break;
    }
    if (itmp > i && itmp < eh->n) {
      i = itmp;
    } else if (i != itmp) {
      fprintf(stderr, "eh->his bad major index, i: %d, read %d\n", i, itmp);
      goto ERR;
    }
    pd = eh->his + i * eh->cnt;
    if (zcom_endn__fread(&jmin, sizeof(jmin), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading jmin\n");
      goto ERR;
    }
    if ( !(jmin >= 0 && jmin < eh->cnt) ) {
      fprintf(stderr, "eh->his: base index %d out of boudary [0, eh->cnt=%d)\n",
          jmin, eh->cnt);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if (zcom_endn__fread(&size, sizeof(size), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading size\n");
      goto ERR;
    }
    if ( !(size > 0 && jmin + size <= eh->cnt) ) {
      fprintf(stderr, "eh->his: invalid size %d, jmin=%d, [0, eh->cnt=%d)\n",
          size, jmin, eh->cnt);
      fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if ((size > 0)
      && (zcom_endn__fread(pd+jmin, sizeof(*(pd+jmin)), size, fp, endn) != (size_t) (size))) {
      fprintf(stderr, "error in reading pd+jmin, n = size(%d)\n", size);
      goto ERR;
    }
  }
  (void) ver;
  return 0;
ERR:
  at_eh__clear(eh);
  return -1;
}

int at_eh__read_binary(at_eh_t *eh, const char *fname, int *pver)
{
  FILE *fp;
  int ver;
  int itmp;
  int i;
  int endn;

  if ( !(eh->mode > 0) ) return 0;
  if ( !(eh->mode == 1) ) {
    fprintf(stderr, "at_eh__read_binary: failed validation: eh->mode == 1\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  if ((fp = fopen(fname, "rb")) == NULL) {
    fprintf(stderr, "cannot read binary file [%s].\n", fname);
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* determine file endian */
  if ((endn = zcom_endn__rmatchi(&itmp, sizeof(int), fp)) < 0) {
    fprintf(stderr, "itmp 0x%X cannot match sizeof(int) 0x%X\n",
        (unsigned) itmp, (unsigned) sizeof(int));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != (int) sizeof(double)) {
    fprintf(stderr, "(int) sizeof(double) mismatch, expect: %d, read: %d, pos: %#lx\n",
        (int) sizeof(double), itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if (zcom_endn__fread(&ver, sizeof(ver), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading ver\n");
    goto ERR;
  }
  if (pver != NULL) *pver = ver;

  /* call low level read function for members */
  i = eh_read_binary_low(eh, fp, ver, endn);
  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}

static int eh_write_binary_low(at_eh_t *eh, FILE *fp, int ver)
{
  int i;
  int jmin;
  int jmax;
  double *pd;
  int size;

  if (eh == NULL) {
    fprintf(stderr, "passing null pointer to eh_write_binary_low\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* n: number of temperature bins */
  if (zcom_endn__fwrite(&eh->n, sizeof(eh->n), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing eh->n\n");
    goto ERR;
  }

  /* eh_cnt: number of energy bins */
  if (zcom_endn__fwrite(&eh->cnt, sizeof(eh->cnt), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing eh->cnt\n");
    goto ERR;
  }
  /* eh_min: minimal energy */
  if (zcom_endn__fwrite(&eh->min, sizeof(eh->min), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing eh->min\n");
    goto ERR;
  }
  /* eh_del: energy bin size */
  if (zcom_endn__fwrite(&eh->del, sizeof(eh->del), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing eh->del\n");
    goto ERR;
  }
  /* eh_his: energy histogram data */
  for (i = 0; i < eh->n; i++) {
    pd = eh->his + i * eh->cnt;
    for (jmax = eh->cnt; jmax > 0 && pd[jmax-1] <= 0.0; jmax--) ;
    for (jmin = 0; jmin < jmax && pd[jmin] <= 0.0; jmin++) ;
    if ((size = jmax - jmin) <= 0) continue;
    if (zcom_endn__fwrite(&i, sizeof(i), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing i\n");
      goto ERR;
    }
    if (zcom_endn__fwrite(&jmin, sizeof(jmin), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing jmin\n");
      goto ERR;
    }
    if (zcom_endn__fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing size\n");
      goto ERR;
    }
    if ((size > 0)
      && (zcom_endn__fwrite(pd+jmin, sizeof(*(pd+jmin)), size, fp, 1) != (size_t) (size))) {
      fprintf(stderr, "error in writing pd+jmin, n = size(%d)\n", size);
      goto ERR;
    }
  }
  (void) ver;
  return 0;
ERR:
  return -1;
}

int at_eh__write_binary(at_eh_t *eh, const char *fname, int ver)
{
  FILE *fp;
  int i;
  int size;

  if ( !(eh->mode > 0) ) return 0;
  if ( !(eh->mode == 1) ) {
    fprintf(stderr, "at_eh__write_binary: failed validation: eh->mode == 1\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  if ((fp = fopen(fname, "wb")) == NULL) {
    fprintf(stderr, "cannot write binary file [%s].\n", fname);
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  size = (int) sizeof(int);
  if (zcom_endn__fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing size\n");
    goto ERR;
  }
  size = (int) sizeof(double);
  if (zcom_endn__fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing size\n");
    goto ERR;
  }
  if (zcom_endn__fwrite(&ver, sizeof(ver), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing ver\n");
    goto ERR;
  }

  /* call low level write function for members */
  i = eh_write_binary_low(eh, fp, ver);
  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}


#endif
