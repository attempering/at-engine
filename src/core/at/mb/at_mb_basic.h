#ifndef AT_MB_BASIC_H__
#define AT_MB_BASIC_H__

/* basic routines for the multiple-bin estimators */

#include "at_mb__def.h"

typedef struct zcom_cfg_t_ zcom_cfg_t;

/* initialize an object of at_mb_t
 * The initial values are taken from configuration file `cfg`
 * when available, otherwise default values are assumed */
int at_mb__cfg_init(at_mb_t *mb, zcom_cfg_t *cfg,
                double boltz,
                const char *data_dir,
                int silent);

/* destroy data members */
void at_mb__finish(at_mb_t *mb);

/* clear data members */
void at_mb__clear(at_mb_t *mb);

#endif
