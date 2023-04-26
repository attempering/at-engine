#ifndef AT__EH_UTILS_H__
#define AT__EH_UTILS_H__


#include "eh__def.h"

void at_eh__add(at_eh_t *eh, int ib, double e);

/* use the integral identity to reconstruct an unbiased histogram (robust method) */
int at_eh__reconstruct(at_eh_t *eh, const char *fname);

#endif
