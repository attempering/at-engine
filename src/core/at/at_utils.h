#ifndef AT_UTILS_H__
#define AT_UTILS_H__

#include "at_basic.h"

at_t *at__open(const char *zcom_cfg_fn, at_bool_t bCPT, at_bool_t open_log, double boltz, double md_time_step, int suffix);

int at__move(at_t *at, llong_t step, at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush);

#endif
