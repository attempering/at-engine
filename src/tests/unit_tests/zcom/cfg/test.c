#include "zcom/zcom__src.h"

int main(void)
{
  zcom_cfg_t *cfg;
  int first_key = 0;
  char *second_key = NULL;
  double third_key = 0;

  cfg = zcom_cfg__open("foo.cfg", ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__ALLOW_DASHES);

  if (zcom_cfg__get(cfg, &first_key, "First-Key", "%d") != 0) {
    fprintf(stderr, "failed to find first-key\n");
  } else {
    printf("first-key: %d\n", first_key);
  }

  if (zcom_cfg__get(cfg, &second_key, "second_key", "%s") != 0) {
    fprintf(stderr, "failed to find second-key\n");
  } else {
    printf("second-key: %s\n", second_key);
  }

  if (zcom_cfg__get(cfg, &third_key, "third-KEY", "%lf") != 0) {
    fprintf(stderr, "failed to find third-key\n");
  } else {
    printf("third-key: %f\n", third_key);
  }

  zcom_cfg__close(cfg);

  return 0;
}
