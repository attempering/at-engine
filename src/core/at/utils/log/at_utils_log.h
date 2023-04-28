#ifndef AT_UTILS_LOG_H__
#define AT_UTILS_LOG_H__

#include "at_utils_log__def.h"

void at_utils_log__cfg_init(at_utils_log_t *log,
    zcom_cfg_t *cfg, int silent);

int at_utils_log__open_file(at_utils_log_t *log);

void at_utils_log__close_file(at_utils_log_t *log);

void at_utils_log__finish(at_utils_log_t *log);

#endif
