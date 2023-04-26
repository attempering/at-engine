#ifndef ZCOM__SSM__SRC_H__
#define ZCOM__SSM__SRC_H__


//#define SSM_DBG__ 1


#include "ssm.h"


ZCOM_STRCLS zcom_ssm_t *zcom_ssm__openx(int hash_bits, size_t block_size)
{
  zcom_ssm_t *m;

  if ((m = (zcom_ssm_t *) calloc(1, sizeof(zcom_ssm_t))) == NULL) {
    fprintf(stderr, "Fatal: no memory for a new zcom_ssm_t object\n");
    return NULL;
  }

  if (hash_bits < 0) {
    hash_bits = SSM_DEFAULT_HASH_BITS;
  }

  m->hash_bits = hash_bits;
  m->hash_size = (size_t) (1uLL << m->hash_bits);

  if (block_size == 0) {
    block_size = SSM_DEFAULT_BLOCK_SIZE;
  }

  m->block_size = block_size;

  if ((m->base_ = (zcom_ssm__ss_llist_node_t *) calloc(m->hash_size, sizeof(zcom_ssm__ss_llist_node_t))) == NULL) {
    fprintf(stderr, "zcom_ssm_t failed to allocate the initial hash table of size %u\n", (unsigned) m->hash_size);
    free(m);
    return NULL;
  }

  {
    size_t i;
    zcom_ssm__ss_llist_node_t *head;

    for (i = 0; i < m->hash_size; i++) {
      head = m->base_ + i;
      head->next = NULL;
      head->size = 0;
      head->s = NULL;
    }
  }

#ifdef SSM_DBG__
  fprintf(stderr, "m->base_ %p\n", m->base_);
  getchar();
#endif

  return m;
}


ZCOM_STRCLS zcom_ssm_t *zcom_ssm__open(void)
{
  return zcom_ssm__openx(-1, 0);
}


/* return the hash value from the string's pointer address */
static size_t zcom_ssm_calc_hash_value_(zcom_ssm_t *m, const char *p)
{
  size_t val = (size_t) p * 1664525u + 1013904223u;
  size_t val2 = (val >> (32 - m->hash_bits));
  size_t mask = ((1 << m->hash_bits) - 1);

#ifdef SSM_DBG__
  fprintf(stderr, "ssm string pointer %p, hash value 0x%lx => 0x%lx mask 0x%lx\n",
      p, (unsigned long) val, (unsigned long) val2, (unsigned long) mask);
#endif

  return val2 & mask;
}


/*
 * return the *previous* header to the one that associates with s
 * first locate the list from the Hash value, then enumerate the linked list.
 * */
static zcom_ssm__ss_llist_node_t *zcom_ssm_llist_find_(zcom_ssm_t *m, const char *s)
{
  zcom_ssm__ss_llist_node_t *hp, *head;

  if (s == NULL) {
    return NULL;
  }

  head = m->base_ + zcom_ssm_calc_hash_value_(m, s);

  for (hp = head; hp->next != NULL; hp = hp->next) {
    if (hp->next->s == s) {
      return hp;
    }
  }

  return NULL;
}



ZCOM_INLINE void zcom_ssm_llist_print_(zcom_ssm_t *m, size_t hash_id, const char* tag)
{
  zcom_ssm__ss_llist_node_t *head = m->base_ + hash_id;
  zcom_ssm__ss_llist_node_t *hp = head;
  fprintf(stderr, "list[%lu, %p] %s:\n", (unsigned long) hash_id, head, tag);

  while (hp->next != NULL) {
    fprintf(stderr, "%p ", hp->next);
    hp = hp->next;
  }

  fprintf(stderr, "\n");
}



/*
 * simply add the entry h at the beginning of the list
 * we do not accept a recalculated hash value,
 * since realloc might have changed it
 * */
static zcom_ssm__ss_llist_node_t *zcom_ssm_llist_add_node_(zcom_ssm_t *m, zcom_ssm__ss_llist_node_t *h)
{
  size_t hash = zcom_ssm_calc_hash_value_(m, h->s);
  zcom_ssm__ss_llist_node_t *head = m->base_ + hash;

#ifdef SSM_DBG__

  fprintf(stderr, "ssm to add node %p for string %p to list[@hash=%lu] for string of size %lu\n",
      h, h->s, (unsigned long) hash, (unsigned long) h->size);
    
  zcom_ssm_llist_print_(m, hash, "zcom_ssm_add BEFORE");
#endif

  /*
    Before:

           head               h1      h2
    next:  h1                 h2      NULL


    After:

           head       h       h1      h2
    next:  h          h1      h2      NULL

   */

  h->next = head->next;
  head->next = h;

#ifdef SSM_DBG__
  zcom_ssm_llist_print_(m, hash, "zcom_ssm_add AFTER");
  getchar();
#endif

  return head;
}


/* remove hp->next */
static void zcom_ssm_llist_remove_node_(zcom_ssm__ss_llist_node_t *hp, int free_node)
{
  zcom_ssm__ss_llist_node_t *h = hp->next;

  hp->next = h->next;
  if (free_node) {
    //fprintf(stderr, "freeing nodes %p & %p\n", (void *) h, (void *) h->s);
    free(h->s);
    free(h);
  }
}


/* (re)allocate memory for (*php)->next, update list, return the new string
 * n is the number of nonempty characters, obtained e.g. from strlen().
 * create a new header if *php is NULL, in this case, the first character
 * of the string is '\0'
 * */
static char *zcom_ssm_resize_string_(zcom_ssm_t *m, zcom_ssm__ss_llist_node_t **php, size_t new_size)
{
  zcom_ssm__ss_llist_node_t *h = NULL, *hn, *hp;
  size_t size;

  if (php == NULL) {
    fprintf(stderr, "zcom_ssm_resize_string_: php is NULL, n = %u", (unsigned) new_size);
    return NULL;
  }

  if ( (hp = *php) == NULL
    || (h = hp->next) == NULL
    || (h->size < new_size + 1) )
  {

    size = ((new_size/m->block_size) + 1) * m->block_size;

    if (h == NULL || size != h->size) {

      if (h == NULL) {

        /* to allocate a new node */
        if ((hn = (zcom_ssm__ss_llist_node_t *) calloc(1, sizeof(zcom_ssm__ss_llist_node_t))) == NULL) {
          fprintf(stderr, "zcom_ssm_resize_string_: no memory for a list node\n");
          return NULL;
        }
  
        /* to allocate space for the string */
        if ((hn->s = (char *) calloc(size, 1)) == NULL) {
          fprintf(stderr, "zcom_ssm_resize_string_: no memory for string of size %lu\n", (long unsigned) size);
        }

#ifdef SSM_DBG__
        fprintf(stderr, "ssm allocates a node %p for string %p of %lu bytes\n", hn, hn->s, (unsigned long) size);
#endif

      } else {

        /* since realloc h->s will change the hash value of h
         * hence its index in m->head
         * we have to first remove the old node h from the linked list */
        zcom_ssm_llist_remove_node_(hp, 0);

        /* to reuse an old node */
        hn = h;

        /* to reallocate space for the string */
        if ((hn->s = (char *) realloc(hn->s, size)) == NULL) {
          fprintf(stderr, "zcom_ssm_resize_string_: no memory for string of size %lu\n", (long unsigned) size);
        }

      }

      hn->size = size;

      *php = hp = zcom_ssm_llist_add_node_(m, hn);
    }

  }

  return hp->next->s;
}



/* delete a string, etc ... */
ZCOM_STRCLS int zcom_ssm__del(zcom_ssm_t *m, char *s)
{
  zcom_ssm__ss_llist_node_t *hp;

  if (s == NULL || (hp = zcom_ssm_llist_find_(m, s)) == NULL) {
    if (s) {
      fprintf(stderr, "zcom_ssm_manage: unknown address %p (%s)\n",  s, s);
    }
    return -1;
  }

  zcom_ssm_llist_remove_node_(hp, 1);

  return 0;
}



/* delete all strings, shrink memory, etc ... */
ZCOM_STRCLS int zcom_ssm__del_all(zcom_ssm_t *m)
{
  zcom_ssm__ss_llist_node_t *hp, *head;
  size_t i;

  for (i = 0; i < m->hash_size; i++)
  {
    hp = head = m->base_ + i;

    if (hp->next == NULL) {
      continue;
    }

#ifdef SSM_DBG__
    zcom_ssm_llist_print_(m, i, "zcom_ssm__delete_all: BEFORE");
#endif

    while (hp->next != NULL)
    {
#ifdef SSM_DBG__
      fprintf(stderr, "%lu: hp %p, hp->next %p\n", (unsigned long) i, hp, hp->next);
#endif

      zcom_ssm_llist_remove_node_(hp, 1);
    }

#ifdef SSM_DBG__
    zcom_ssm_llist_print_(m, i, "zcom_ssm__delete_all: AFTER");
#endif

  }

  return 0;
}


ZCOM_STRCLS void zcom_ssm__close(zcom_ssm_t *m)
{
  zcom_ssm__del_all(m);
  free(m->base_);
  free(m);
}


/*
 * copy/cat t to *ps
 *
 * If (flags & SSM_TASK_CONCAT) == 0:
 * copy t to *ps, if ps is not NULL, and return the result
 * if ps or *ps is NULL, we return a string created from t
 *   *ps is set to the same value if ps is not NULL
 * otherwise, we update the record that corresponds to *ps
 *
 * min_size: to request a minimal size for the resulting buffer
 *
 * If flags & SSM_TASK_CONCAT:
 * append t after *ps. Equivalent to cpy if ps or *ps is NULL.
 * */
ZCOM_STRCLS char *zcom_ssm__copy_or_concat(zcom_ssm_t *m, char **ps, const char *t, size_t min_size, unsigned flags)
{
  zcom_ssm__ss_llist_node_t *hp = NULL;
  size_t t_size = 0u, s_size = 0u;
  char *s = NULL, *p;

  /* both ps and *ps can be NULL, in which cases we leave hp as NULL */
  if (ps != NULL && (s = *ps) != NULL && (hp = zcom_ssm_llist_find_(m, s)) == NULL) {
    fprintf(stderr, "zcom_ssm__copy_or_concat: unknown address %p (%s)\n", s, s);
    return NULL;
  }

  if (t != NULL) {
    while (t[t_size]) { /* compute the length of t */
      t_size++;
    }
  }

  if (flags & SSM_TASK_CONCAT) {
    if (s != NULL) { /* s is also NULL, if ps itself is NULL */
      while (s[s_size]) { /* compute the length of s */
        s_size++;
      }
    }
    t_size += s_size;
  }  /* s_size is always 0 in case of copying */

  if (t_size < min_size) {
    t_size = min_size;
  }

  if ((s = zcom_ssm_resize_string_(m, &hp, t_size)) == NULL) { /* change size */
    return NULL;
  }

  if (t != NULL) {
    for (p = s + s_size; (*p++ = *t++); ) { /* copy/cat the string */
      ;
    }
  }

  if (ps != NULL) {
    *ps = s;
  }

  return s;
}


ZCOM_STRCLS char *zcom_ssm__copy(zcom_ssm_t *m, char **ps, const char *t)
{
  return zcom_ssm__copy_or_concat(m, ps, t, 0, 0);
}


ZCOM_STRCLS char *zcom_ssm__concat(zcom_ssm_t *m, char **ps, const char *t)
{
  return zcom_ssm__copy_or_concat(m, ps, t, 0, SSM_TASK_CONCAT);
}


ZCOM_STRCLS char *zcom_ssm__dup(zcom_ssm_t *m, const char *t)
{
  return zcom_ssm__copy(m, NULL, t);
}


ZCOM_STRCLS char *zcom_ssm__new(zcom_ssm_t *m)
{
  return zcom_ssm__dup(m, NULL);
}



/* get a string *ps from file fp
 * *ps can be NULL, in which case memory is allocated
 * *pn is number of characters read (including '\n', but not the terminal null)
 * delim is the '\n' for reading a singe line
 * */
ZCOM_STRCLS char *zcom_ssm__fgetx(zcom_ssm_t *m, char** ps, size_t *pn, int delim, FILE *fp)
{
  size_t n, max;
  int c;
  char *s;
  zcom_ssm__ss_llist_node_t *hp;

  if (ps == NULL || fp == NULL) {
    return NULL;
  }

  if ((s = *ps) == NULL) { /* allocate an initial buffer if *ps is NULL */
    if ((s = zcom_ssm__copy(m, ps, NULL)) == NULL)
      return NULL;
  }

  if ((hp = zcom_ssm_llist_find_(m, s)) == NULL) {
    fprintf(stderr, "ssfgetx: unknown address %p (%s)\n", s, s);
    return NULL;
  }

  max = hp->next->size - 1;

  for (n = 0; (c = fgetc(fp)) != EOF; ) {

    if (n+1 > max) { /* request space for n+1 non-blank characters */
      if ((*ps = s = zcom_ssm_resize_string_(m, &hp, n+1)) == NULL) {
        return NULL;
      }

      max = hp->next->size - 1;
    }

    s[n++] = (char)(unsigned char) c;

    if (c == delim) {
      break;
    }

  }

  s[n] = '\0';

  if (pn != NULL) {
    *pn = n;
  }

  return (n > 0) ? s : NULL;
}


ZCOM_STRCLS char *zcom_ssm__fgets(zcom_ssm_t *m, char **ps, size_t *pn, FILE *fp)
{
  return zcom_ssm__fgetx(m, ps, pn, '\n', fp);
}


ZCOM_STRCLS char *zcom_ssm__fget_all(zcom_ssm_t *m, char **ps, size_t *pn, FILE *fp)
{
  return zcom_ssm__fgetx(m, ps, pn, EOF, fp);
}


#endif /* ZCOM__SSM_H_ */
