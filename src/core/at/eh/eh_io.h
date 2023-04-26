#ifndef AT__EH_IO_H__
#define AT__EH_IO_H__

#include "eh_def.h"

#include "eh_io_binary.h"

int eh__read(eh_t *eh);

int eh__write(eh_t *eh);

void eh__manifest(const eh_t *eh, FILE *fp, int arrmax);

#endif
