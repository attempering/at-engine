#ifndef AT_LANGEVIN_BASIC_H__
#define AT_LANGEVIN_BASIC_H__

#include "at_langevin__def.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;
typedef struct at_mb_t_ at_mb_t;

int at_langevin__cfg_init(at_langevin_t *langevin,
    at_mb_t *mb, zcom_cfg_t *cfg, int silent);

void at_langevin__clear(at_langevin_t *langevin);

void at_langevin__finish(at_langevin_t *langevin);

#endif
