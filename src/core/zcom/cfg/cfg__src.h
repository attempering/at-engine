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

#ifndef ZCOM__CFG__SRC_H__
#define ZCOM__CFG__SRC_H__


#include "cfg.h"

#define zcom_cfg__set(cfg, var) zcom_opt__isset(cfg->opts, cfg->nopt, &var, #var)


static char *zcom_cfg__standardize_key(zcom_cfg_t *cfg, const char *key_)
{
  char *key = zcom_ssm__dup(cfg->ssm, key_);
  char *p, *q, *key2;

  if (cfg->flags & ZCOM_CFG__IGNORE_CASE) {
    /* convert every letter to lower case */
    for (p = key; *p != '\0'; p++) {
      *p = zcom_util__tolower(*p);
    }
  }

  if (cfg->flags & ZCOM_CFG__IGNORE_DASHES) {

    key2 = zcom_ssm__dup(cfg->ssm, key_);

    p = key2;

    for (q = key; *q != '\0'; q++) {
      if (strchr("-_", *q) == NULL) {
        *p++ = *q;
      }
    }
    *p = '\0';

    key = key2;

  } else if (cfg->flags & ZCOM_CFG__ALLOW_DASHES) {
    /* converting '_' to '-' */
    for (p = key; *p != '\0'; p++) {
      if (*p == '_') {
        *p = '-';
      }
    }
  }

  return key;
}


/* Comparing keys,

   user_key is the key in the configuration file
   registered_key is the key given by the programmer
 */
static int zcom_cfg__compare_keys(const char *user_key, const char *registered_key)
{
  return strcmp(user_key, registered_key);
}

static int zcom_cfg__match_multiple_keys(const char *user_key, size_t n, char **registered_keys)
{
  size_t i;

  for (i = 0; i < n; i++) {
    if (zcom_cfg__compare_keys(user_key, registered_keys[i]) == 0) {
      return 1;
    }
  }

  return 0;
}


static char **zcom_cfg__break_down_keys(zcom_cfg_t *cfg, const char *key_, size_t *nkeys)
{
  size_t i;
  char *p;
  const char *delims = ",;";
  char *key, **keys;

  key = zcom_ssm__dup(cfg->ssm, key_);

  // figure out the number of keys;
  i = 1;
  for (p = key; *p != '\0'; p++) {
    if (strchr(delims, *p) != NULL) {
      i++;
    }
  }
  *nkeys = i;

  keys = (char **) calloc(*nkeys, sizeof(char *));

  keys[0] = key;

  i = 1;

  for (p = key; *p != '\0'; p++) {
    if (strchr(delims, *p) != NULL) {
      keys[i++] = p + 1;
    }
  }

  for (i = 0; i < *nkeys; i++) {
    zcom_util__strip(keys[i]);
    keys[i] = zcom_cfg__standardize_key(cfg, keys[i]);
  }

  return keys;
}



/* Read the value of a given variable from the current configuration file,
 * the name of variable is given by `key`,
 *
 * If the key is matched, its value is saved to `*var' through sscanf,
 * otherwise, the content in *var is not modified.
 *
 * If the function succeeds, it returns 0.
 *
 * In case fmt is "%s", (*var) is a string, or a pointer to char,
 * i.e., var should be of the type of `char **`.
 * The space for (*var) will be managed through zcom_ssm__copy().
 * 
 **/
int zcom_cfg__get(zcom_cfg_t *cfg, void *var, const char *key, const char *fmt)
{
  int i;
  size_t nkeys = 0;
  char **keys = 0;

  if (cfg == NULL) {
    return -1;
  }

  keys = zcom_cfg__break_down_keys(cfg, key, &nkeys);

  if (keys == NULL) {
    return -1;
  }

  for (i = 0; i < cfg->nent; i++) {
    zcom_cfgent_t *ent = cfg->ents + i;

    if (ent->key != NULL && zcom_cfg__match_multiple_keys(ent->key, nkeys, keys)) {
      //fprintf(stderr, "found a matched key [%s] vs [%s]\n", key, ent->key);

      if (strcmp(fmt, "%s") == 0) { /* string */
        //fprintf(stderr, "trying to get a string value [%s] : %s\n", key, (var ? *(char**)var : NULL));

        /* make a copy and return */
        char *val_str = zcom_ssm__dup(cfg->ssm, ent->val);
        if (var != NULL) {
          *(char **) var = val_str;
        }

        //fprintf(stderr, "after assignment [%s]: %s\n", key, (var ? *(char**)var : NULL));
        free(keys);

        return 0;

      } else { /* use sscanf for other cases, like int, float,... */

        int ret_val = (EOF == sscanf(ent->val, fmt, var) ? 2 : 0);

        free(keys);

        return ret_val;
      }
    }
  }


  free(keys);

  return 1; /* no match */
}


/* load the whole configuration file into memory, parse it to entries */
zcom_cfg_t *zcom_cfg__open(const char *fn, unsigned flags)
{
  zcom_cfg_t *cfg;
  zcom_cfgent_t *ent;
  FILE *fp;
  size_t i, j, n, size = 0;
  char *p, *q;

  /* a comment line may start with one of the following characters
   *
   * # % ! ;
   *
   */
  const char *comment_chars = "#%!;";


  zcom_util__exit_if ((cfg = (zcom_cfg_t *) calloc(1, sizeof(zcom_cfg_t))) == NULL,
    "Fatal@zcom.cfg: no memory for a new zcom_cfg_t object\n");

  cfg->ssm = NULL;

  cfg->flags = flags;

  if ((fp = fopen(fn, "r")) == NULL) {
    fprintf(stderr, "Error@zcom.cfg: failed to open (read) %s\n", fn);
    free(cfg);
    return NULL;
  }

  cfg->ssm = zcom_ssm__open();

  if (zcom_ssm__fget_all(cfg->ssm, &(cfg->buf), &size, fp) == NULL) {
    fprintf(stderr, "Error@zcom.cfg: error reading file %s\n", fn);
    return NULL;
  }

  /* add a new line if the file does not end with one */
  zcom_ssm__concat(cfg->ssm, &(cfg->buf), "\n");
  fclose(fp);

  /* count the number of lines (before allocating the key-table) */
  for (p = cfg->buf, i = 0, n = 0; i < size; i++) {
    if (p[i] == '\n' || p[i] == '\r') {
      if (i > 0 && p[i-1] == '\\') {
        /* multiple-line splicing by replacing cr, lf with spaces
           parse should be aware of these additional spaces */
        p[i-1] = p[i] = ' ';
      } else {
        p[i] = '\0';
        n++;
      }

      /* replace following CR LF by spaces for efficiency
         as the size of the key table == the number of blank lines */
      for (j = i+1; j < size && zcom_util__isspace(p[j]); j++) {
        p[j] = ' ';
      }
    }
  }
  n++; /* for the last line */

  if ((cfg->ents = (zcom_cfgent_t *) calloc(n, sizeof(*cfg->ents))) == NULL) {
    fprintf(stderr, "Error@zcom.cfg: no memory for %lu objects of zcom_cfgent_t\n", (unsigned long) n);
    exit(1);
  }

  /* load lines into the key table
   * do not parse the `key = value` yet */
  for (p = cfg->buf, j = 0, i = 0; i < size; i++) {
    if (cfg->buf[i] == '\0') {
      cfg->ents[j++].key = p;
      //fprintf(stderr, "I got %s\n", p);
      if (j >= n) {
        break;
      }
      p = cfg->buf + i + 1;
    }
  }
  cfg->ents[j++].key = p;
  //fprintf(stderr, "I got %s\n", p);
  n = j;
  //fprintf(stderr, "I got %d lines\n", n); getchar();

  /* parse each line to a key-value pair */
  for (j = 0; j < n; j++) {

    ent = cfg->ents + j;
    p = ent->key; /* unparsed line */

    for (q = p; *q != '\0'; q++) {
      if (strchr(comment_chars, *q)) {
        *q = '\0';
        break;
      }
    }

    zcom_util__strip(p);

    /* skip a blank or comment line */
    if (p[0] == '\0') {
      ent->key = NULL;
      continue;
    }

    if ((q = strchr(p, '=')) == NULL) { /* skip a line without '=' */
      ent->key = NULL;
      continue;
    }

    *q = '\0';
    ent->val = q + 1;

    zcom_util__strip(ent->key);

    ent->key = zcom_cfg__standardize_key(cfg, ent->key);

    zcom_util__strip(ent->val);

  }

  cfg->nent = (int) n;
  cfg->nopt = 0;
  cfg->nopt_cap = ZCOM_CFG__OPT_BLOCK_SIZE_;
  if ((cfg->opts = (zcom_opt_t *) calloc(cfg->nopt_cap, sizeof(zcom_opt_t))) == NULL) {
    fprintf(stderr, "Error@zcom.cfg: no memory for %d objects of op_t", cfg->nopt_cap);
    exit(1);
  }

  return cfg;
}


void zcom_cfg__close(zcom_cfg_t *cfg)
{
  if (cfg == NULL) {
    return;
  }

  free(cfg->ents);
  free(cfg->opts);
  zcom_ssm__close(cfg->ssm);
  memset(cfg, 0, sizeof(*cfg));
  free(cfg);
}


/* register an option request, return the index */
ZCOM__INLINE int zcom_cfg__add(zcom_cfg_t *cfg, const char *key, const char *fmt, void *ptr, const char *desc)
{
  int n = cfg->nopt++;
  zcom_opt_t *o;
  char *key_std;

  if (cfg->nopt > cfg->nopt_cap) {
    cfg->nopt_cap += ZCOM_CFG__OPT_BLOCK_SIZE_;
    if ((cfg->opts = (zcom_opt_t *) realloc(cfg->opts, cfg->nopt_cap * sizeof(zcom_opt_t))) == NULL) {
      fprintf(stderr, "Fatal: no memory for %d objects of zcom_opt_t\n", cfg->nopt_cap);
      exit(1);
    }
  }

  o = cfg->opts + n;

  key_std = zcom_cfg__standardize_key(cfg, key);

  zcom_opt__set(o, NULL, key_std, fmt, ptr, desc);

  return n;
}


/* match requested options with entries in cfg file
 * returns 0 if successful */
ZCOM__INLINE int zcom_cfg__match(zcom_cfg_t *cfg, unsigned flags)
{
  int i, j, ret = 0, verbose = flags & ZCOM_CFG__VERBOSE, must = flags & ZCOM_OPT__MANDATORY;
  zcom_opt_t *o;
  zcom_cfgent_t *ent;

  for (i = 0; i < cfg->nopt; i++) {
    o = cfg->opts + i;
    for (j = 0; j < cfg->nent; j++) {
      ent = cfg->ents + j;
      if (ent->key != NULL && zcom_cfg__compare_keys(ent->key, o->key) == 0) {
        ent->used = 1;
        o->flags |= ZCOM_OPT__SET;
        o->val = ent->val;
        zcom_opt__getval(o, cfg->ssm);
        break;
      }
    }
    if (!(o->flags & ZCOM_OPT__SET) && (must || verbose)) {
      printf("cfg: %s not set, default: ", o->key);
      zcom_opt__print_ptr(o);
      printf("\n");
      if (must) ret = 1;
    }
  }

  if (flags & ZCOM_CFG__CHECK_USE) {
    for (j = 0; j < cfg->nent; j++) {
      ent = cfg->ents + j;
      if (ent->key != NULL && !ent->used && verbose)
        printf("cfg: unused entry: %s = %s\n", ent->key, ent->val);
    }
  }
  return ret;
}


#endif