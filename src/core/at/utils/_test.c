#include "utils__src.h"
#include <stdio.h>

int main(void)
{
  ssm_t *ssm = ssm_open();

  char *fn = utils_make_output_filename(ssm, "data1", "a.dat");

  FILE *fp = fopen(fn, "w");
  if (fp == NULL) {
    fprintf(stderr, "failed to open file %s\n", fn);
  } else {
    fprintf(fp, "hello\n");
    fclose(fp);
  }

  ssm_close(ssm);

  return 0;
}