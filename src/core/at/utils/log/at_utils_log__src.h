#ifndef AT_UTILS_LOG__SRC_H__
#define AT_UTILS_LOG__SRC_H__

#include "at_utils_log.h"

#include "../../../zcom/zcom.h"



void at_utils_log__cfg_init(at_utils_log_t *log,
    zcom_cfg_t *cfg, int silent)
{
  log->filename = "log.dat";
  if (cfg != NULL && zcom_cfg__get(cfg, &log->filename, "log_file", "%s"))
  {
    fprintf(stderr, "assuming default: log->filename = \"log.dat\", key: log_file\n");
  }

}



int at_utils_log__open_file(at_utils_log_t *log)
{
  if ((log->fp = fopen(log->filename, "w")) == NULL) {
    return -1;
  }
  return 0;
}


void at_utils_log__close_file(at_utils_log_t *log)
{
  if (log->fp != NULL) {
    fclose(log->fp);
    log->fp = NULL;
  }
}


void at_utils_log__finish(at_utils_log_t *log)
{
  at_utils_log__close_file(log);
}


#endif
