/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT__MB_IO_H__
#define AT__MB_IO_H__


#include "mb_io_binary.h"


/* load previous mb data */
int mb__read(mb_t *mb, langevin_t *langevin);

/* prepare and write mb data  */
int mb__write(mb_t *mb, langevin_t *langevin);

/* mb__manifest: clear mb_t data */
void mb__manifest(const mb_t *mb, FILE *fp, int arrmax);

#endif
