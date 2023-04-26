#ifndef ZCOM__OPT__SRC_H_
#define ZCOM__OPT__SRC_H_


#include "opt.h"

/* translate string values to actual ones through sscanf() */
ZCOM_INLINE int opt_getval(opt_t *o, ssm_t *m)
{
  const char *fmt = o->fmt;

  if (fmt == NULL || fmt[0] == '\0') { /* raw string assignment */
    *(const char **)o->ptr = o->val;
  } else if (strcmp(fmt, "%s") == 0) {
    ssm_copy(m, (char **)o->ptr, o->val);
  } else { /* call sscanf */

    if (strcmp(fmt, "%r") == 0) { /* real */
      fmt = (sizeof(real) == sizeof(float)) ? "%f" : "%lf";
    }

    if (1 != sscanf(o->val, fmt, o->ptr)) {
      fprintf(stderr, "Error: unable to convert a value for [%s] as fmt [%s], raw string: [%s]\n",
          o->desc, fmt, o->val);
      return 1;
    }
  }

  return 0;
}

/* set properties of an option: fmt = "%b" for a switch */
ZCOM_INLINE void opt_set(opt_t *o, const char *sflag, const char *key,
    const char *fmt, void *ptr, const char *desc)
{
  o->ch = '\0';
  if (key) {
    o->isopt = 2;
  } else if (sflag) { /* option */
    o->isopt = 1;
    o->ch = (char) ( sflag[2] ? '\0' : sflag[1] ); /* no ch for a long flag */
  } else { /* argument */
    o->isopt = 0;
  }
  o->sflag = sflag;
  o->key = key;
  o->flags = 0;
  exit_if (ptr == NULL, "null pass to argopt with %s: %s\n", sflag, desc);
  o->ptr = ptr;
  if (fmt == NULL) fmt = "";
  if (fmt[0] == '!') {
    fmt++;
    o->flags |= OPT_MUST;
  }
  if (strcmp(fmt, "%b") == 0) {
    fmt = "%d";
    o->flags |= OPT_SWITCH;
  }
  o->fmt = fmt;
  o->pfmt = NULL;
  o->desc = desc;
}

/* print the value of o->ptr */
ZCOM_INLINE void opt_printptr(opt_t *o)
{
  const char *fmt;

  for (fmt = o->fmt; *fmt && *fmt != '%'; fmt++) ;

#define ELIF_PF_(fm, fmp, type) else if (strcmp(fmt, fm) == 0) \
  printf((o->pfmt ? o->pfmt : fmp), *(type *)o->ptr)

  if (fmt == NULL || *fmt == '\0') printf("%s", (*(char **)o->ptr) ? (*(char **)o->ptr) : "NULL");
  ELIF_PF_("%b", "%d", int); /* switch */
  ELIF_PF_("%d", "%d", int);
  ELIF_PF_("%u", "%u", unsigned);
  ELIF_PF_("%x", "0x%x", unsigned);
  ELIF_PF_("%ld", "%ld", long);
  ELIF_PF_("%lu", "%lu", unsigned long);
  ELIF_PF_("%lx", "0x%lx", unsigned long);
#if 0  /* C99 only */
  ELIF_PF_("%lld", "%lld", long long);
  ELIF_PF_("%llu", "%llu", unsigned long long);
  ELIF_PF_("%llx", "0x%llx", unsigned long long);
#endif
  ELIF_PF_("%f", "%g", float);
  ELIF_PF_("%lf", "%g", double);
  ELIF_PF_("%r", "%g", real);
  else printf("unknown %s-->%%d: %d\n", fmt, *(int *)o->ptr);
#undef ELIF_PF_
}

/* search an option list, return an option whose variable address is p */
ZCOM_INLINE opt_t *opt_find(opt_t *ls, int n, const void *p)
{
   int i;
   for (i = 0; i < n; i++) if (ls[i].ptr == p) return ls + i;
   return NULL;
}

/* search an option list to see if an option is explicitly set */
ZCOM_INLINE int opt_isset(opt_t *ls, int n, const void *p, const char *var)
{
  opt_t *o = opt_find(ls, n, p);
  exit_if (!o, "cannot find var %s, ptr %p\n", var, p);
  return o->flags & OPT_SET ? 1 : 0;
}

#endif
