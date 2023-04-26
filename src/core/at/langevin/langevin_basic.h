#ifndef AT__LANGEVIN_BASIC_H__
#define AT__LANGEVIN_BASIC_H__

#include "langevin__def.h"

typedef struct cfg_t_ cfg_t;

int langevin_cfg_init(langevin_t *langevin, cfg_t *cfg, int silent);

void langevin_clear(langevin_t *langevin);

void langevin_finish(langevin_t *langevin);

#endif
