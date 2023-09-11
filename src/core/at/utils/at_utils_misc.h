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

#ifndef AT_UTILS_MISC_H__
#define AT_UTILS_MISC_H__

#include "sys/at_utils_sys.h"
#include "at_utils__def.h"

const char *at_utils__bool_to_str(at_bool_t val);

char *at_utils__make_output_filename(zcom_ssm_t *ssm, const char *data_dir, const char *fn);

#ifndef at_utils__new_arr
#define at_utils__new_arr(x, n, type) \
  if (sizeof(*(x)) != sizeof(type)) { \
    fprintf(stderr, "Error@at.utils: declared type `%s` does not match variable `%s`\n", #type, #x); \
    exit(1); \
  } else if ((x = (type *) calloc(n, sizeof(*(x)))) == NULL) { \
    fprintf(stderr, "Error@at.utils: no memory for `%s` (%s) x %lu\n", \
        #x, #type, (unsigned long) (n)); \
    exit(1); \
}
#endif

#ifndef at_utils__new
#define at_utils__new(x, type) at_utils__new_arr(x, 1, type)
#endif


#endif
