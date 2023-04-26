#ifndef ZCOM__LOG_H__
#define ZCOM__LOG_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../def/def.h"

typedef struct logfile_t_ {
  FILE *fp;
  const char *fname;
  unsigned flags;
} logfile_t;

#define LOG_WRITESCREEN  0x01
#define LOG_FLUSHAFTER   0x02
#define LOG_NOWRITEFILE  0x10
#define LOG_APPEND       0x80

ZCOM_INLINE logfile_t *log_open(const char *fn);

ZCOM_INLINE int log_printf(logfile_t *log, char *fmt, ...);

ZCOM_INLINE void log_close(logfile_t *log);


/* close & reopen log file to make sure that stuff is written to disk */
ZCOM_INLINE int log_hardflush(logfile_t *log);

#if defined(ZCOM_HAVE_VAM) && defined(NEED_WTRACE)
ZCOM_STRCLS logfile_t log_stock_[1] = {{ NULL, "TRACE", 0 }};
#define wtrace(fmt, ...) { \
  if (fmt) log_printf(log_stock_, fmt, ##__VA_ARGS__); \
  else if (log_stock_->fp) { fclose(log_stock_->fp); log_stock_->fname = NULL; } }
#endif


#endif