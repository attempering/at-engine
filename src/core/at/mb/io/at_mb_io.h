/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT_MB_IO_H__
#define AT_MB_IO_H__


#include "at_mb_io_binary.h"


/* load previous mb data */
int at_mb__read(at_mb_t *mb, at_langevin_t *langevin, double *beta);

/* prepare and write mb data  */
int at_mb__write(at_mb_t *mb, at_langevin_t *langevin, double beta);

/* at_mb__manifest: clear at_mb_t data */
void at_mb__manifest(const at_mb_t *mb, FILE *fp, int arrmax);

#endif
