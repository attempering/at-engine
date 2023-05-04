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

#ifndef ZCOM__ENDN_H__
#define ZCOM__ENDN_H__



#include <stdio.h>
#include <string.h>

#include "../def/def.h"

ZCOM__STRCLS int zcom_endn__system(void);
ZCOM__STRCLS size_t zcom_endn__fwrite(void *ptr, size_t size, size_t n, FILE *fp, int endn);
ZCOM__STRCLS size_t zcom_endn__fread(void *ptr, size_t size, size_t n, FILE *fp, int flip);
ZCOM__STRCLS int zcom_endn__rmatch(void *src, const void *ref, size_t size, FILE *fp);
ZCOM__STRCLS int zcom_endn__rmatchi(int *src, int iref, FILE *fp);

#endif