/* 
 * Copyright (C) 2010-2023  C. Zhang
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

#ifndef ZCOM__SSM_H__
#define ZCOM__SSM_H__


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



ZCOM__STRCLS zcom_ssm_t *zcom_ssm__openx(int hash_bits, size_t block_size);


ZCOM__STRCLS zcom_ssm_t *zcom_ssm__open(void);

/* delete a string */
ZCOM__STRCLS int zcom_ssm__del(zcom_ssm_t *m, char *s);

/* delete all strings */
ZCOM__STRCLS int zcom_ssm__del_all(zcom_ssm_t *m);

ZCOM__STRCLS void zcom_ssm__close(zcom_ssm_t *m);


/*
 * copy/cat t to *ps
 *
 * If (flags & ZCOM_SSM__TASK_CONCAT) == 0:
 * copy t to *ps, if ps is not NULL, and return the result
 * if ps or *ps is NULL, we return a string created from t
 *   *ps is set to the same value if ps is not NULL
 * otherwise, we update the record that corresponds to *ps
 *
 * min_size: to request a minimal size for the resulting buffer
 *
 * If flags & ZCOM_SSM__TASK_CONCAT:
 * append t after *ps. Equivalent to cpy if ps or *ps is NULL.
 * */
ZCOM__STRCLS char *zcom_ssm__copy_or_concat(zcom_ssm_t *m, char **ps, const char *t, size_t min_size, unsigned flags);

ZCOM__STRCLS char *zcom_ssm__copy(zcom_ssm_t *m, char **ps, const char *t);

ZCOM__STRCLS char *zcom_ssm__concat(zcom_ssm_t *m, char **ps, const char *t);

ZCOM__STRCLS char *zcom_ssm__dup(zcom_ssm_t *m, const char *t);

ZCOM__STRCLS char *zcom_ssm__new(zcom_ssm_t *m);



/* get a string *ps from file fp
 * *ps can be NULL, in which case memory is allocated
 * *pn is number of characters read (including '\n', but not the terminal null)
 * delim is the '\n' for reading a singe line
 * */
ZCOM__STRCLS char *zcom_ssm__fgetx(zcom_ssm_t *m, char** ps, size_t *pn, int delim, FILE *fp);


ZCOM__STRCLS char *zcom_ssm__fgets(zcom_ssm_t *m, char **ps, size_t *pn, FILE *fp);

ZCOM__STRCLS char *zcom_ssm__fget_all(zcom_ssm_t *m, char **ps, size_t *pn, FILE *fp);

#endif