#ifndef AT__EH_BASIC_H__
#define AT__EH_BASIC_H__

#include "eh_def.h"

typedef struct cfg_t_ cfg_t;

/* initialize an object of eh_t
 * The initial values are taken from configuration file `cfg`
 * when available, otherwise default values are assumed */
int eh__cfg_init(eh_t *eh, cfg_t *cfg, mb_t *mb, const char *data_dir);

/* clear data */
void eh__clear(eh_t *eh);

void eh__finish(eh_t *eh);

#endif
