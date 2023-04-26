#ifndef AT__AT_BASIC_H__
#define AT__AT_BASIC_H__

#include "at__def.h"

#define at_beta_to_temp(x, boltz) (1.0/(boltz * x))
#define at_temp_to_beta(x, boltz) (1.0/(boltz * x))

int at_load_data(at_t *at, at_bool_t bCPT);

int at_do_every(llong_t step, int nst, at_bool_t bfirst, at_bool_t blast);

void at_output(at_t *at, llong_t step,
    int ib, double invw, double t1, double t2, double Eav,
    at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush);

/* initialize members of at_t from the configuration
 * file `cfg`, or if unavailable, from default values */
int at_cfg_init(at_t *at, zcom_cfg_t *cfg, double boltz, double tmstep);

void at_finish(at_t *at);

/* close a pointer to at_t */
void at_close(at_t *at);

int at_manifest(at_t *at, const char *fn, int arrmax);

#endif
