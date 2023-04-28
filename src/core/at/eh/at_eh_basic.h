#ifndef AT_EH_BASIC_H__
#define AT_EH_BASIC_H__

#include "at_eh__def.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;

/* initialize an object of at_eh_t
 * The initial values are taken from configuration file `cfg`
 * when available, otherwise default values are assumed */
int at_eh__cfg_init(at_eh_t *eh,
    at_mb_t *mb, zcom_cfg_t *cfg, const char *data_dir);

/* clear data */
void at_eh__clear(at_eh_t *eh);

void at_eh__finish(at_eh_t *eh);

#endif
