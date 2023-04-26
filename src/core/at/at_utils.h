#ifndef AT__AT_UTILS_H__
#define AT__AT_UTILS_H__

#include "at_basic.h"

at_t *at_open(const char *cfg_fn, at_bool_t bCPT, at_bool_t open_log, double boltz, double tmstep, int suffix);

int at_move(at_t *at, llong_t step, at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush);

#endif
