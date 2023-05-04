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


int zcom_cfg__compare_keys(const char *user_key, const char *registered_keys)
{
  return strcmp(user_key, registered_keys);
}


/* Read the value of a given variable from the current configuration file,
 * the name of variable is given by `key`,
 * If the key is matched, its value is saved to `*var' through sscanf,
 *   otherwise, the content in *var is not modified.
 *
 * If the function succeeds, it returns 0.
 *
 * In case fmt is "%s", (*var) is a string, or a pointer to char.
 *   The space for (*var) will be managed through zcom_ssm__copy(). */
int zcom_cfg__get(zcom_cfg_t *cfg, void *var, const char *key, const char *fmt)
{
  int i;

  if (cfg == NULL) {
    return -1;
  }

  for (i = 0; i < cfg->nent; i++) {
    zcom_cfgent_t *ent = cfg->ents + i;

    if (ent->key != NULL && zcom_cfg__compare_keys(ent->key, key) == 0) {
      //fprintf(stderr, "found a matched key [%s] vs [%s]\n", key, ent->key);

      if (strcmp(fmt, "%s") == 0) { /* string */
        //fprintf(stderr, "trying to get a string value [%s] : %s\n", key, (var ? *(char**)var : NULL));

        /* make a copy and return */
        char *val_str = zcom_ssm__dup(cfg->ssm, ent->val);
        if (var != NULL) {
          *(char **) var = val_str;
        }

        //fprintf(stderr, "after assignment [%s]: %s\n", key, (var ? *(char**)var : NULL));
        return 0;
      } else { /* use sscanf for other cases, like int, float,... */
        return EOF == sscanf(ent->val, fmt, var) ? 2 : 0;
      }
    }
  }

  return 1; /* no match */
}

/* load the whole configuration file into memory, parse it to entries */
zcom_cfg_t *zcom_cfg__open(const char *fn)
{
  zcom_cfg_t *cfg;
  zcom_cfgent_t *ent;
  FILE *fp;
  size_t i, j, n, size = 0;
  char *p, *q;

  if ((cfg = (zcom_cfg_t *) calloc(1, sizeof(zcom_cfg_t))) == NULL) {
    fprintf(stderr, "Fatal: no memory for a new zcom_cfg_t object\n");
    return NULL;
  }

  cfg->ssm = zcom_ssm__open();

  zcom_util__xfopen(fp, fn, "r", return NULL);
  if (zcom_ssm__fget_all(cfg->ssm, &(cfg->buf), &size, fp) == NULL) {
    fprintf(stderr, "error reading file %s\n", fn);
    return NULL;
  }
  zcom_ssm__concat(cfg->ssm, &(cfg->buf), "\n"); /* in case the file is not ended by a new line, we add one */
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
      for (j = i+1; j < size && zcom_util__cisspace(p[j]); j++) {
        p[j] = ' ';
      }
    }
  }
  n++; /* for the last line */

  if ((cfg->ents = (zcom_cfgent_t *) calloc(n, sizeof(*cfg->ents))) == NULL) {
    fprintf(stderr, "Fatal: no memory for %lu objects of zcom_cfgent_t\n", (unsigned long) n);
    exit(1);
  }

  /* load lines into the key table */
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
    p = ent->key;
    zcom_util__strip(p);

    /* skip a blank or comment line */
    if (p[0] == '\0' || strchr("#%!;", p[0]) != NULL) {
      ent->key = NULL;
      continue;
    }

    /* remove trailing space and ';' */
    for (q = p + strlen(p) - 1; q >= p && (zcom_util__cisspace(*q) || *q == ';'); q--) {
      *q = '\0';
    }

    if ((q = strchr(p, '=')) == NULL) { /* skip a line without '=' */
      ent->key = NULL;
      continue;
    }
    *q = '\0';
    ent->val = q + 1;
    zcom_util__strip(ent->key);
    zcom_util__strip(ent->val);
  }
  cfg->nent = (int) n;
  cfg->nopt = 0;
  cfg->nopt_cap = ZCOM_CFG__OPT_BLOCK_SIZE_;
  if ((cfg->opts = (zcom_opt_t *) calloc(cfg->nopt_cap, sizeof(zcom_opt_t))) == NULL) {
    fprintf(stderr, "Fatal no memory for %d objects of op_t", cfg->nopt_cap);
    exit(1);
  }

  return cfg;
}

void zcom_cfg__close(zcom_cfg_t *cfg)
{
  free(cfg->ents);
  free(cfg->opts);
  zcom_ssm__close(cfg->ssm);
  memset(cfg, 0, sizeof(*cfg));
  free(cfg);
}

/* register an option request, return the index */
int zcom_cfg__add(zcom_cfg_t *cfg, const char *key, const char *fmt, void *ptr, const char *desc)
{
  int n = cfg->nopt++;
  zcom_opt_t *o;

  if (cfg->nopt > cfg->nopt_cap) {
    cfg->nopt_cap += ZCOM_CFG__OPT_BLOCK_SIZE_;
    if ((cfg->opts = (zcom_opt_t *) realloc(cfg->opts, cfg->nopt_cap * sizeof(zcom_opt_t))) == NULL) {
      fprintf(stderr, "Fatal: no memory for %d objects of zcom_opt_t\n", cfg->nopt_cap);
      exit(1);
    }
  }

  o = cfg->opts + n;
  zcom_opt__set(o, NULL, key, fmt, ptr, desc);

  return n;
}

/* match requested options with entries in cfg file
 * returns 0 if successful */
int zcom_cfg__match(zcom_cfg_t *cfg, unsigned flags)
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