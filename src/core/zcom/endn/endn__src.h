/* 
 * Copyright (C) 2006-2023  AT-Engine Developers
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

#ifndef ZCOM__ENDN__SRC_H__
#define ZCOM__ENDN__SRC_H__



#include "endn.h"


/* return the system endian, 1: big endian, 0: little endian */
int zcom_endn__system(void)
{
  unsigned feff = 0xFEFF; /* assume unsigned is at least 16-bit */
  unsigned char *p;

  p  = (unsigned char *) &feff;
  return (*p == 0xFF) ? 0 : 1;
}

/* change endianness in-place for n items of size in ptr */
ZCOM__INLINE void endn_flip(void *ptr, size_t size, size_t n)
{
  unsigned char *p = (unsigned char *) ptr, ch;
  size_t i, r, half = size/2;

  for (; n > 0; n--, p += size) {
    /* reverse bytes for each object */
    for (i = 0; i < half; i++) {
      r = size - i - 1;
      ch   = p[i];
      p[i] = p[r];
      p[r] = ch;
    }
  }
}

/* write data in ptr to file with a specific endian 'endn'
 * `ptr` is not const, because it needs to change its endian */
size_t zcom_endn__fwrite(void *ptr, size_t size, size_t n, FILE *fp, int endn)
{
  static int endn_sys = -1;

  /* initial determine the machine's endianess */
  if (endn_sys < 0) {
    endn_sys = zcom_endn__system();
  }
  if (endn == endn_sys) {
    return fwrite(ptr, size, n, fp);
  }

  endn_flip(ptr, size, n);
  n = fwrite(ptr, size, n, fp);
  endn_flip(ptr, size, n);
  return n;
}

/* read an object test object *src, compared with *ref
 * return 0 if they are identical without endian change
 * return 1 if changing the endianness of *src matches *ref
 * otherwise return -1 */
int zcom_endn__rmatch(void *src, const void *ref, size_t size, FILE *fp)
{
  if (1 != fread(src, size, 1, fp))
    return -1;
#ifdef ENDN_DBG
  if (size == sizeof(int))
    printf("A: 0x%X vs. 0x%X size = %u, cmp = %d\n",
      *(int *)src, *(int *)ref, (unsigned)size,
      memcmp(src, ref, size));
#endif
  if (memcmp(src, ref,  size) == 0)
    return 0;
  /* alter the endianness, and test again */
  endn_flip(src, size, 1);
#ifdef ENDN_DBG
  if (size == sizeof(int))
    printf("B: 0x%X vs. 0x%X size = %u, cmp = %d\n",
      *(int *)src, *(int *)ref, (unsigned)size,
      memcmp(src, ref, size));
#endif
  return (memcmp(src, ref, size) == 0) ? 1 : -1;
}

/* special case of zcom_endn__rmatchi for integer, convenient because
 * iref could be e.g. sizeof(int), which has no address */
int zcom_endn__rmatchi(int *src, int iref, FILE *fp)
{
  return zcom_endn__rmatch(src, &iref, sizeof(int), fp);
}

/* read data from file to ptr with endianness changed if 'flip' is 1
 * flip can be initialized by calling zcom_endn__rmatch() for a test object */
size_t zcom_endn__fread(void *ptr, size_t size, size_t n, FILE *fp, int flip)
{
  n = fread(ptr, size, n, fp);
  if (flip) endn_flip(ptr, size, n);
  return n;
}


#endif
