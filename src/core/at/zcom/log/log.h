#ifndef ZCOM__LOG_H__
#define ZCOM__LOG_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../def/def.h"

typedef struct at_logfile_t_ {
  FILE *fp;
  const char *fname;
  unsigned flags;
} at_logfile_t;

#define ZCOM_LOG_WRITESCREEN  0x01
#define ZCOM_LOG_FLUSHAFTER   0x02
#define ZCOM_LOG_NOWRITEFILE  0x10
#define ZCOM_LOG_APPEND       0x80

ZCOM_INLINE at_logfile_t *zcom_log__open(const char *fn);

ZCOM_INLINE int zcom_log__printf(at_logfile_t *log, char *fmt, ...);

ZCOM_INLINE void zcom_log__close(at_logfile_t *log);


/* close & reopen log file to make sure that stuff is written to disk */
ZCOM_INLINE int zcom_log__hard_flush(at_logfile_t *log);


#endif