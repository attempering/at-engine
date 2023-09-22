#include "zcom/zcom__src.h"

static int test_ignore_dashes(const char *fn)
{
  zcom_cfg_t *cfg;
  int first_key = 0;
  char *second_key = NULL;
  double third_key = 0;

  cfg = zcom_cfg__open(fn, NULL, ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__IGNORE_DASHES);

  if (zcom_cfg__get(cfg, &first_key, "First-Key", "%d") != 0) {
    fprintf(stderr, "failed to find first-key from %s\n", fn);
  } else {
    printf("first-key: %d\n", first_key);
  }

  if (zcom_cfg__get(cfg, &second_key, "second--key-is", "%s") != 0) {
    fprintf(stderr, "failed to find second--key-is from %s\n", fn);
  } else {
    printf("second--key-is: %s\n", second_key);
  }

  if (zcom_cfg__get(cfg, &third_key, "third-KEY", "%lf") != 0) {
    fprintf(stderr, "failed to find third-key from %s\n", fn);
  } else {
    printf("third-key: %f\n", third_key);
  }

  zcom_cfg__close(cfg);

  return 0;
}


static int test_allow_dashes(const char *fn)
{
  zcom_cfg_t *cfg;
  int first_key = 0;
  char *second_key = NULL;
  double third_key = 0;

  cfg = zcom_cfg__open(fn, NULL, ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__ALLOW_DASHES);

  if (zcom_cfg__get(cfg, &first_key, "First-Key", "%d") != 0) {
    fprintf(stderr, "failed to find first-key from %s\n", fn);
  } else {
    printf("first-key: %d\n", first_key);
  }

  if (zcom_cfg__get(cfg, &second_key, "second-key, second-key-alternative, second-key-alt", "%s") != 0) {
    fprintf(stderr, "failed to find second-key from %s\n", fn);
  } else {
    printf("second-key: %s\n", second_key);
  }

  if (zcom_cfg__get(cfg, &third_key, "third-KEY", "%lf") != 0) {
    fprintf(stderr, "failed to find third-key from %s\n", fn);
  } else {
    printf("third-key: %f\n", third_key);
  }

  zcom_cfg__close(cfg);

  return 0;
}


int main(void)
{
  test_ignore_dashes("ignore-dashes.cfg");
  test_allow_dashes("allow-dashes.cfg");

  return 0;
}