#include "utils___src.h"
#include <stdio.h>

int main(void)
{
  zcom_ssm_t *ssm = zcom_ssm__open();

  char *fn = at_utils__make_output_filename(ssm, "data1", "a.dat");

  FILE *fp = fopen(fn, "w");
  if (fp == NULL) {
    fprintf(stderr, "failed to open file %s\n", fn);
  } else {
    fprintf(fp, "hello\n");
    fclose(fp);
  }

  zcom_ssm__close(ssm);

  return 0;
}