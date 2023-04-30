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

#ifndef AT_CONTEXT__DEF_H__
#define AT_CONTEXT__DEF_H__


/* This module defines the programming context:
   It determines whether we have defined `real` and `gmx_bool`
   as in the GROMACS environment.

   It also defines at_bool_t and at_llong_t
*/


#if defined(AT__GROMACS)

  #include "gromacs/at_context_gromacs__def.h"

#else

  #include "ansi/at_context_ansi__def.h"

#endif /* defined(AT__GROMACS) */



#ifndef AT__FALSE
#define AT__FALSE 0
#endif

#ifndef AT__TRUE
#define AT__TRUE 1
#endif


/* define a long long int type */
#ifndef AT_INT32_ONLY
typedef long long  at_llong_t;
#define at_llong_pfmt "%lld"
#else
typedef long at_llong_t;
#define at_llong_pfmt "%ld"
#endif


#endif
