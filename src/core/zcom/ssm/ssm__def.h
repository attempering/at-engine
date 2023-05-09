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

#ifndef ZCOM__SSM__DEF_H__
#define ZCOM__SSM__DEF_H__


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../def/def.h"



enum {
  ZCOM_SSM__TASK_CONCAT = 1,
  ZCOM_SSM__TASK_DELETE = 2,
};

enum {
  ZCOM_SSM__DEFAULT_HASH_BITS = 8,
  ZCOM_SSM__DEFAULT_BLOCK_SIZE = 256,
};

typedef struct zcom_ssm__ss_llist_node_t_ {
  size_t size;
  char *s;
  struct zcom_ssm__ss_llist_node_t_ *next;
} zcom_ssm__ss_llist_node_t;

typedef zcom_ssm__ss_llist_node_t zcom_ssm__ss_llist_t;


typedef struct zcom_ssm_t_ {
  int hash_bits;
  size_t hash_size;
  size_t block_size; /* minimum number of bytes */
  zcom_ssm__ss_llist_t *base_; /* base_[0..bash_size-1]*/
} zcom_ssm_t;



#endif
