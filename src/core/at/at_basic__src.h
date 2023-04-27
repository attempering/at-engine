#ifndef AT__AT_BASIC__SRC_H__
#define AT__AT_BASIC__SRC_H__

#include <unistd.h>

#include "at_basic.h"

#include "../zcom/zcom.h"

#include "mb/mb.h"
#include "langevin/langevin.h"
#include "eh/eh.h"


/* load previous data */
int at__load_data(at_t *at, at_bool_t bCPT)
{
  at_mb_t *mb = at->mb;
  int read_mb = 1;

  if (!bCPT) { /* initial run */
    return 0;
  }

  /*read_mb = (at->premode > 0) ? 0 : bCPT;*/
  read_mb = bCPT;
  if (read_mb) {
     /* read previous at_mb_t data */
    if (at_mb__read(mb, at->langevin, &at->beta) != 0) {
      fprintf(stderr, "cannot load mb data from %s\n", mb->av_file);
      return 1;
    }

    at_mb__write_ze_file(mb, "ze_init.dat");

    /* read previous energy histogram data */
    if (at_eh__read(at->eh) != 0) {
      fprintf(stderr, "cannot load energy histogram from %s\n", at->eh->file);
      return 2;
    }
  }

  fprintf(stderr, "successfully load previous data\n");

  return 0;
}



/* don't write at the first step; write at the last step */
int at__do_every(llong_t step, int nst, at_bool_t bfirst, at_bool_t blast)
{
  return !bfirst && (blast || (nst > 0 && step % nst == 0));
}



/* write various output files */
void at__output(at_t *at, llong_t step,
          int ib, double invw, double t1, double t2, double Eav,
          at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush)
{
  at_bool_t do_log;

  /* write the trace file */
  if (at->nst_log > 0) {
    do_log = (step % at->nst_log == 0) || bfirst || blast;
  } else { /* tracing is disabled if at->nst_log < 0 */
    do_log = (at->nst_log == 0) ? btr : 0;
  }

  //fprintf(stderr, "%d, %d %p\n", do_log, at->nst_log, at->log);getchar();

  if (at->log) {

    if(bflush) {
      at->log->flags |= ZCOM_LOG__FLUSH_AFTER;
    } else {
      at->log->flags ^= ZCOM_LOG__FLUSH_AFTER;
    }

    if (do_log) {
      zcom_log__printf(at->log,
          "%10.3f %5d %10.6f %12.3f %12.3f %10.6f %8.3f %8.5f",
          step * at->md_time_step, ib, t2 - t1,
          at->energy, Eav, at->beta, t1, invw);
      zcom_log__printf(at->log, "\n");
    }
  }

  if (at__do_every(step, at->mb->nst_save_av, bfirst, blast)) { /* save averages */
    at_mb__write(at->mb, at->langevin, at->beta);
    at_mb__write_ze_file(at->mb, NULL);
    if (at->mtrng) {
      zcom_mtrng__save(at->mtrng, at->rng_file);
    }
  }

  if (at__do_every(step, at->eh->nst_save, bfirst, blast)) { /* save energy histograms */
    if (at->eh != NULL) { 
      at_eh__write(at->eh);
      at_eh__reconstruct(at->eh, NULL);
    }
  }

}



int at__cfg_init(at_t *at, zcom_cfg_t *cfg, double boltz, double md_time_step)
{
  /* temp_thermostat: thermostat temperature */
  at->temp_thermostat = 300.0;
  if (zcom_cfg__get(cfg, &at->temp_thermostat, "T0", "%lf")) {
    fprintf(stderr, "assuming default: at->temp_thermostat = 300.0, key: T0\n");
  }

  /* nsttemp: frequency of tempering, 0: disable, -1: only ns */
  at->nsttemp = -1;
  if (zcom_cfg__get(cfg, &at->nsttemp, "nsttemp", "%d")) {
    fprintf(stderr, "assuming default: at->nsttemp = -1, key: nsttemp\n");
  }

  /* md_time_step: MD integration step, for convenience */
  at->md_time_step = md_time_step;

  /* nst_log: interval of writing trace file; -1: only when doing neighbor search, 0: disable */
  at->nst_log = -1;
  if (zcom_cfg__get(cfg, &at->nst_log, "nsttrace", "%d")) {
    fprintf(stderr, "assuming default: at->nst_log = -1, key: nsttrace\n");
  }

  {
    char *fn_rng = "mtrng.dat";

    /* rng_file: file name of random number state */
    if (zcom_cfg__get(cfg, &fn_rng, "rng_file", "%s")) {
      fprintf(stderr, "assuming default: at->rng_file = \"%s\", key: rng_file\n", fn_rng);
    }

    at->rng_file = at_utils__make_output_filename(at->ssm, at->data_dir, fn_rng);
  }

  {
    char *fn_log = "trace.dat";

    /* log_file: name of trace file */
    if (zcom_cfg__get(cfg, &fn_log, "log_file", "%s"))
    {
      fprintf(stderr, "assuming default: at->log_file = \"%s\", key: log_file\n", fn_log);
    }

    at->log_file = at_utils__make_output_filename(at->ssm, at->data_dir, fn_log);
    //fprintf(stderr, "log file %s => %s\n", fn_log, at->log_file);getchar();
  }

  /* log: logfile */
  // do not open log file yet, at__open_log();
  at->log = NULL;

  int silent = 0;

  at_bias__cfg_init(at->bias, cfg, silent);

  /* handler for multiple-bin estimator */
  at_mb__cfg_init(at->mb, cfg, boltz, at->data_dir, silent);

  at_langevin__cfg_init(at->langevin, at->mb, cfg, silent);

  /* energy histogram */
  at_eh__cfg_init(at->eh, at->mb, cfg, at->data_dir);

  at->energy = 0.0;

  return 1;
}


void at__finish(at_t *at)
{
  if (at->mpi_rank == 0) {
    if (at->mtrng) {
      zcom_mtrng__save(at->mtrng, at->rng_file);
      zcom_mtrng__close(at->mtrng);
    }
  }

  if (at->mpi_rank == 0) {
    zcom_log__close(at->log);
  }

  if (at->eh != NULL) {
    at_eh__finish(at->eh);
  }

  at_mb__finish(at->mb);

  at_langevin__finish(at->langevin);

  at_bias__finish(at->bias);

  zcom_ssm__close(at->ssm);

  memset(at, 0, sizeof(*at));
}


void at__close(at_t *at)
{
  at__finish(at);
  free(at);
}


int at__manifest(at_t *at, const char *fn, int arrmax)
{
  FILE *fp;

  if ((fp = fopen(fn, "w")) == NULL)
  {
      fprintf(stderr, "cannot write %s\n", fn);
      return -1;
  }


  fprintf(fp, "at->temp_thermostat: double, %g\n", at->temp_thermostat);
  fprintf(fp, "at->nsttemp: int, %4d\n", at->nsttemp);
  fprintf(fp, "at->md_time_step: double, %g\n", at->md_time_step);
  fprintf(fp, "at->rng_file: char *, %s\n", at->rng_file);

  fprintf(fp, "at->nst_log: int, %4d\n", at->nst_log);

  if (at->log) {
    fprintf(fp, "at->log->fname: char *, %s\n", at->log->fname);
  }

  at_bias__manifest(at->bias, fp, arrmax);

  if (at->mb != NULL) {
    fprintf(fp, "at->mb: object pointer to at_mb_t\n");
    at_mb__manifest(at->mb, fp, arrmax);
  }

  if (at->eh != NULL) {
    fprintf(fp, "at->eh: object pointer to at_eh_t\n");
    at_eh__manifest(at->eh, fp, arrmax);
  }

  if (at->langevin != NULL) {
    fprintf(fp, "at->langevin: object pointer to at_langevin_t\n");
    at_langevin__manifest(at->langevin, fp, arrmax);
  }

  fprintf(fp, "at->energy: double, %g\n", at->energy);

  fclose(fp);

  return 0;
}



#endif
