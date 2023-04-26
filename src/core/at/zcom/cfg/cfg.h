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
} zcom_cfgent_t; /* line from cfg file */

typedef struct zcom_cfg_t_ {
  char *buf;      /* the string buffer of the entire configuration file */
  int nent;       /* number of entries */
  zcom_cfgent_t *ents; /* entries */
  int nopt;       /* number of user-requested options */
  int nopt_cap;   /* capacity of opts[] */
  zcom_opt_t *opts;    /* user-requested options */
  zcom_ssm_t *ssm;
} zcom_cfg_t;

#define CFG_CHECKUSE 0x0100
#define CFG_VERBOSE  0x1000
#define CFG_OPT__BLOCK_SIZE_ 64

ZCOM_STRCLS zcom_cfg_t *zcom_cfg__open(const char *fn);
ZCOM_STRCLS void zcom_cfg__close(zcom_cfg_t *cfg);
ZCOM_STRCLS int zcom_cfg__add(zcom_cfg_t *cfg, const char *key, const char *fmt, void *ptr, const char *desc);
ZCOM_STRCLS int zcom_cfg__match(zcom_cfg_t *cfg, unsigned flags);
ZCOM_STRCLS int zcom_cfg__get(zcom_cfg_t *cfg, void *var, const char *key, const char *fmt);

#endif
