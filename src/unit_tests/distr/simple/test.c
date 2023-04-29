#include "at/distr/at_distr__src.h"
#include "at/utils/at_utils__src.h"
#include "zcom/zcom__src.h"

int main(void)
{
  at_distr_t distr[1];
  zcom_cfg_t *cfg = NULL;
  at_utils_manifest_t manifest[1];

  at_distr__cfg_init(distr, cfg, 1);
  at_distr__manifest(distr, manifest);
  at_distr__finish(distr);

  return 0;
}