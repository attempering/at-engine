#ifndef AT__EH_BASIC_H__
#define AT__EH_BASIC_H__

#include "eh__def.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;

/* initialize an object of at_eh_t
 * The initial values are taken from configuration file `cfg`
 * when available, otherwise default values are assumed */
int at_eh__cfg_init(at_eh_t *eh, zcom_cfg_t *cfg, at_mb_t *mb, const char *data_dir);

/* clear data */
void at_eh__clear(at_eh_t *eh);

void at_eh__finish(at_eh_t *eh);

#endif
