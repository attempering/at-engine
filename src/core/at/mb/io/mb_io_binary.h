/* binary IO routines for mb_t */
#ifndef AT__MB_IO_BINARY_H__
#define AT__MB_IO_BINARY_H__

#include "../mb__def.h"
#include "../../langevin/langevin.h"


/* mb__read_binary: read mb_t data as binary */
int mb__read_binary(mb_t *mb, langevin_t *langevin, const char *fname, int *pver);

/* mb__write_binary: write mb_t data as binary */
int mb__write_binary(mb_t *mb, langevin_t *langevin, const char *fname, int ver);

#endif
