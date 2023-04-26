#ifndef AT__EH_UTILS_H__
#define AT__EH_UTILS_H__


#include "eh_def.h"

void eh__add(eh_t *eh, int ib, double e);

/* use the integral identity to reconstruct an unbiased histogram (robust method) */
int eh__reconstruct(eh_t *eh, const char *fname);

#endif
