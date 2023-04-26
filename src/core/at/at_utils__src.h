#ifndef AT__AT_UTILS__SRC_H__
#define AT__AT_UTILS__SRC_H__


#include "at_utils.h"

#include "zcom/util/util.h"
#include "zcom/rng/rng.h"
#include "utils/utils.h"



/* return a pointer of an initialized at_t
 * if possible, initial values are taken from configuration
 * file `cfg`, otherwise default values are assumed */
static at_t *at_cfg_open(const char *cfgname, double boltz, double tmstep, int isuffix)
{
  cfg_t *cfg;
  at_t *at;
  at_bool_t bLoaded;

  /* open configuration file */
  exit_if(!(cfg = cfg_open(cfgname)),
      "at_t: cannot open configuration file %s.\n", cfgname);

  /* allocate memory for at_t */
  exit_if ((at = (at_t *) calloc(1, sizeof(at_t))) == NULL,
      "Fatal: no memory for a new object of at_t\n");

  at->boltz = boltz;

  /* Get the file suffix first */
  exit_if(isuffix >= 10, "do not support # of simulations > 10 currently\n");
  at->ch_suffix = (char)(((int)'0') + isuffix);

  at->ssm = ssm_open();

  sprintf(at->data_dir, "atdata%d", isuffix);
  printf("datadir: %s\n", at->data_dir);

  /* call low level function */
  exit_if (!(bLoaded = at_cfg_init(at, cfg, boltz, tmstep)),
    "at_t: error while reading configuration file %s\n", cfgname);

  fprintf(stderr, "Successfully loaded configuration file %s!\n", cfgname);

  /* load random number generator */
  at->mtrng = mtrng_open(0);
  mtrng_load_or_init_from_seed(at->mtrng, at->rng_file, 0);

  /* close handle to configuration file */
  cfg_close(cfg);

  return at;
}


at_t *at_open(
    const char *cfg_fn,
    at_bool_t bCPT,
    at_bool_t open_log,
    double boltz,
    double time_step,
    int suffix)
{
  at_t *at;

  /* this will also initialize settings for member objects such as at->mb */
  at = at_cfg_open((cfg_fn != NULL) ? cfg_fn : "at.cfg", boltz, time_step, suffix);
  exit_if(at == NULL, "failed to load configuration file.\n");

  fprintf(stderr, "initial temperature set to %g, beta %g\n", at->T0, at_beta_to_temp(at->T0, at->boltz));

  /* make the initial temperature = T0 */
  at->beta = at_temp_to_beta(at->T0, at->boltz);
  at->mb->beta = at->beta;

  /* we only load previous data if it's continuation */
  if (at_load_data(at, bCPT) != 0) {
    fprintf(stderr, "Warning: This simulation is started from checkpoint, while some files are missing. Will assume no previous simulation data is available.\n");
  }

  if (open_log && at->log_file != NULL) {
    at->log = log_open(at->log_file);
  }

  return at;
}



int at_move(at_t *at, llong_t step, at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush)
{
  double invwf = 1.0, temp1, temp2, Eav = 0.0, neg_dlnwf_dbeta;
  int ib, rep;

  temp1 = at_beta_to_temp(at->beta, at->boltz);

  ib = mb__beta_to_index(at->mb, at->beta, 1);

  /* update energy data, change at->beta */
  /* repeat several times to change the temperature */
  for (rep = 0; rep < at->mvreps; rep++) {
    /* 1. deposit the current energy and temperature */
    mb__add(at->mb, at->Ea, at->beta, &ib, &invwf, &neg_dlnwf_dbeta);

    /* 2. add the current data point to the energy histogram */
    eh__add(at->eh, ib, at->Ea);

    /* 3. use the Langevin equation to update the temperature */
    at->beta = langevin__move(at->langevin, at->mb, at->Ea, at->beta,
        ib, invwf, neg_dlnwf_dbeta, at->mtrng, &Eav);
  }

  temp2 = at_beta_to_temp(at->beta, at->boltz);

  if (at_do_every(step, at->mb->nstrefresh, 0, blast)) {
    mb__refresh_et(at->mb, 1);
  }

  at_output(at, step, ib, invwf, temp1, temp2, Eav, bfirst, blast, btr, bflush);

  return 0;
}


#endif
