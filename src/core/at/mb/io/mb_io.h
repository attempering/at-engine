/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT__MB_IO_H__
#define AT__MB_IO_H__


#include "mb_io_binary.h"


/* load previous mb data */
int at_mb__read(at_mb_t *mb, at_langevin_t *langevin);

/* prepare and write mb data  */
int at_mb__write(at_mb_t *mb, at_langevin_t *langevin);

/* at_mb__manifest: clear at_mb_t data */
void at_mb__manifest(const at_mb_t *mb, FILE *fp, int arrmax);

#endif
