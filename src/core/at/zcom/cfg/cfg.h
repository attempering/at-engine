#ifndef ZCOM__CFG_H__
#define ZCOM__CFG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../def/def.h"
#include "../opt/opt.h"
#include "../ssm/ssm.h"

typedef struct {
  char *key, *val;
  int used;
} cfgent_t; /* line from cfg file */

typedef struct cfg_t_ {
  char *buf;      /* the string buffer of the entire configuration file */
  int nent;       /* number of entries */
  cfgent_t *ents; /* entries */
  int nopt;       /* number of user-requested options */
  int nopt_cap;   /* capacity of opts[] */
  opt_t *opts;    /* user-requested options */
  ssm_t *ssm;
} cfg_t;

#define CFG_CHECKUSE 0x0100
#define CFG_VERBOSE  0x1000
#define CFG_OPT__BLOCK_SIZE_ 64

ZCOM_STRCLS cfg_t *cfg_open(const char *fn);
ZCOM_STRCLS void cfg_close(cfg_t *cfg);
ZCOM_STRCLS int cfg_add(cfg_t *cfg, const char *key, const char *fmt, void *ptr, const char *desc);
ZCOM_STRCLS int cfg_match(cfg_t *cfg, unsigned flags);
ZCOM_STRCLS int cfg_get(cfg_t *cfg, void *var, const char *key, const char *fmt);

#endif
