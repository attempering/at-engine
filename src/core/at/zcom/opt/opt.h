#ifndef ZCOM__OPT_H__
#define ZCOM__OPT_H__


#include <stdio.h>

#include "../def/def.h"
#include "../util/util.h"

/* option either from arguments or configuration */
typedef struct zcom_opt_t_ {
  int isopt; /* is option or argument */
  char ch; /* single letter option flag */
  const char *sflag; /* long string flag */
  const char *key; /* key */

  const char *val; /* raw string from command line */
  const char *desc; /* description */
  const char *fmt; /* sscanf format */
  const char *pfmt; /* printf format, NULL: to guess */
  void *ptr; /* address to the target variable */
  unsigned flags;
} zcom_opt_t;

#define ZCOM_OPT_MUST     0x0001  /* a mandatory argument or option */
#define ZCOM_OPT_SWITCH   0x0002  /* an option is a switch */
#define ZCOM_OPT_SET      0x0004  /* an argument/option is set */

/* translate string values to actual ones through sscanf() */
ZCOM_INLINE int zcom_opt__getval(zcom_opt_t *o, zcom_ssm_t *m);

/* set properties of an option: fmt = "%b" for a switch */
ZCOM_INLINE void zcom_opt__set(zcom_opt_t *o, const char *sflag, const char *key,
    const char *fmt, void *ptr, const char *desc);

/* print the value of o->ptr */
ZCOM_INLINE void zcom_opt__print_ptr(zcom_opt_t *o);

/* search an option list, return an option whose variable address is p */
ZCOM_INLINE zcom_opt_t *zcom_opt__find(zcom_opt_t *ls, int n, const void *p);

/* search an option list to see if an option is explicitly set */
ZCOM_INLINE int zcom_opt__isset(zcom_opt_t *ls, int n, const void *p, const char *var);

#endif