#ifndef AT__MB_BASIC_H__
#define AT__MB_BASIC_H__

/* basic routines for the multiple-bin estimators */

#include "mb__def.h"

typedef struct cfg_t_ cfg_t;

/* initialize an object of mb_t
 * The initial values are taken from configuration file `cfg`
 * when available, otherwise default values are assumed */
int mb__cfg_init(mb_t *mb, cfg_t *cfg,
                double boltz, double bmin, double bmax,
                const char *data_dir,
                int silent);

/* destroy data members */
void mb__finish(mb_t *mb);

/* clear data members */
void mb__clear(mb_t *mb);

#endif
