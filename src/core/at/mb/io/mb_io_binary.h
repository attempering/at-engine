/* binary IO routines for at_mb_t */
#ifndef AT__MB_IO_BINARY_H__
#define AT__MB_IO_BINARY_H__

#include "../mb__def.h"
#include "../../langevin/langevin.h"


/* at_mb__read_binary: read at_mb_t data as binary */
int at_mb__read_binary(
    at_mb_t *mb,
    at_langevin_t *langevin,
    double *beta,
    const char *fname,
    int *pver);


/* at_mb__write_binary: write at_mb_t data as binary */
int at_mb__write_binary(
    at_mb_t *mb,
    at_langevin_t *langevin,
    double beta,
    const char *fname,
    int ver);

#endif
