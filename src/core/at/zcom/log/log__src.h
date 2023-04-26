#ifndef ZCOM__LOG__SRC_H__
#define ZCOM__LOG__SRC_H__


#include "log.h"


ZCOM_INLINE at_logfile_t *zcom_log__open(const char *fn)
{
  at_logfile_t *log;

  if ((log = (at_logfile_t *) calloc(1, sizeof(*log))) == NULL) {
    fprintf(stderr, "Fatal: no memory for a new at_logfile_t object\n");
    exit(1);
  }

  if (fn == NULL) {
    fn = "LOG";
  }
  log->fname = fn;
  log->flags = 0;
  log->flags |= ZCOM_LOG_APPEND;
  return log;
}

ZCOM_INLINE int zcom_log__printf(at_logfile_t *log, char *fmt, ...)
{
  va_list args;

  if (log == NULL) return 1;
  if (log->fp == NULL) {
    const char *aw = (log->flags & ZCOM_LOG_APPEND) ? "a" : "w";
    if ((log->fp = fopen(log->fname, aw)) == NULL) {
      fprintf(stderr, "failed to open %s\n", log->fname);
      return 1;
    }
  }
  if ((log->flags & ZCOM_LOG_NOWRITEFILE) == 0) {
    va_start(args, fmt);
    vfprintf(log->fp, fmt, args);
    va_end(args);
  }
  if (log->flags & ZCOM_LOG_WRITESCREEN) {
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
  if (log->flags & ZCOM_LOG_FLUSHAFTER)
    fflush(log->fp);
  return 0;
}

ZCOM_INLINE void zcom_log__close(at_logfile_t *log)
{
  if (log == NULL) {
    return;
  }

  if (log->fp != NULL) {
    fclose(log->fp);
    log->fp = NULL;
  }

  free(log);
}


/* close & reopen log file to make sure that stuff is written to disk */
ZCOM_INLINE int zcom_log__hard_flush(at_logfile_t *log)
{
  if (log->fp == NULL || log->fname == NULL) return 1;

  fclose(log->fp);

  if ((log->fp = fopen(log->fname, "a")) == NULL) {
    fprintf(stderr, "failed to open %s\n", log->fname);
    return 1;
  }

  return 0;
}

#if defined(ZCOM_HAVE_VAM) && defined(NEED_WTRACE)
ZCOM_STRCLS at_logfile_t log_stock_[1] = {{ NULL, "TRACE", 0 }};
#define wtrace(fmt, ...) { \
  if (fmt) zcom_log__printf(log_stock_, fmt, ##__VA_ARGS__); \
  else if (log_stock_->fp) { fclose(log_stock_->fp); log_stock_->fname = NULL; } }
#endif


#endif