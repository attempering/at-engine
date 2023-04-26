#ifndef AT__LANGEVIN_IO__SRC_H__
#define AT__LANGEVIN_IO__SRC_H__

#include "langevin_io.h"

#include "../zcom/endn/endn.h"

void langevin_manifest(const langevin_t *langevin, FILE *fp, int arrcnt)
{
  fprintf(fp, "langevin->dt: double, %g\n", langevin->dt);

  fprintf(fp, "langevin->dTmax: double, %g\n", langevin->dTmax);

  fprintf(fp, "langevin->rejects: double, %g\n", langevin->rejects);

  fprintf(fp, "langevin->total: double, %g\n", langevin->total);

  fprintf(fp, "langevin->corrected: int, %d\n", langevin->corrected);

  fprintf(fp, "langevin->no_skip: int, %d\n", langevin->no_skip);

  fprintf(fp, "langevin->bin_min_visits: double, %lf\n", langevin->bin_min_visits);

  fprintf(fp, "langevin->integrate_func: %p\n", langevin->integrate_func);
}



int langevin_read_binary(langevin_t *langevin, FILE *fp, int endn)
{
  /* rate */
  double rate = 0.0;
  if (endn_fread(&rate, sizeof(rate), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading rate\n");
    goto ERR;
  }

  /* total: total number of attempts of using langevin equation */
  if (endn_fread(&langevin->total, sizeof(langevin->total), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading langevin->total\n");
    goto ERR;
  }
  if ( !((langevin->rejects=langevin->total*rate) >= 0.0) ) {
    fprintf(stderr, "langevin->total: failed validation: (langevin->rejects=langevin->total*rate) >= 0.0\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  return 0;

ERR:
  return 1;
}



int langevin_write_binary(langevin_t *langevin, FILE *fp)
{
  /* rate */
  double rate = (langevin->total > 1.0) ? (langevin->rejects/langevin->total) : 0.0;
  if (endn_fwrite(&rate, sizeof(rate), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing langevin->rate\n");
    goto ERR;
  }

  /* total: total number of attempts of using langevin equation */
  if (endn_fwrite(&langevin->total, sizeof(langevin->total), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing langevin->total\n");
    goto ERR;
  }

  return 0;

ERR:

  return 1;
}

#endif