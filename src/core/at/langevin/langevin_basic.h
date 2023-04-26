#ifndef AT__LANGEVIN_BASIC_H__
#define AT__LANGEVIN_BASIC_H__

#include "langevin__def.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;

int at_langevin__cfg_init(at_langevin_t *langevin, zcom_cfg_t *cfg, int silent);

void at_langevin__clear(at_langevin_t *langevin);

void at_langevin__finish(at_langevin_t *langevin);

#endif
