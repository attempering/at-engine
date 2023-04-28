#ifndef AT_EH_BINARY_IO_H__
#define AT_EH_BINARY_IO_H__

#include "at_eh__def.h"

/* at_eh__read_binary: read at_mb_t/eh data as binary */
int at_eh__read_binary(at_eh_t *eh, const char *fname, int *pver);

/* at_eh__write_binary: write at_mb_t/eh data as binary */
int at_eh__write_binary(at_eh_t *eh, const char *fname, int ver);

#endif
