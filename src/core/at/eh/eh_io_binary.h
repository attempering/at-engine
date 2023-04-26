#ifndef AT__EH_BINARY_IO_H__
#define AT__EH_BINARY_IO_H__

#include "eh_def.h"

/* eh__read_binary: read mb_t/eh data as binary */
int eh__read_binary(eh_t *eh, const char *fname, int *pver);

/* eh__write_binary: write mb_t/eh data as binary */
int eh__write_binary(eh_t *eh, const char *fname, int ver);

#endif
