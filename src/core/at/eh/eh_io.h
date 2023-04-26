#ifndef AT__EH_IO_H__
#define AT__EH_IO_H__

#include "eh__def.h"

#include "eh_io_binary.h"

int at_eh__read(at_eh_t *eh);

int at_eh__write(at_eh_t *eh);

void at_eh__manifest(const at_eh_t *eh, FILE *fp, int arrmax);

#endif
