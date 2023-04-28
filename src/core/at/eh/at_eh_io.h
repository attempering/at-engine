#ifndef AT_EH_IO_H__
#define AT_EH_IO_H__

#include "at_eh__def.h"

#include "at_eh_io_binary.h"

int at_eh__read(at_eh_t *eh);

int at_eh__write(at_eh_t *eh);

void at_eh__manifest(const at_eh_t *eh, FILE *fp, int arrmax);

#endif
