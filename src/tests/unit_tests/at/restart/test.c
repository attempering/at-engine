/* 
 * Copyright (C) 2010-2023  AT-Engine Developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* testing if a restart does not change the trajectory */


//#define AT_MB_DBG__ 1
#include "at-engine__src.h"



double boltz = 1.0;


/* parameters of the Gaussian energy model */
double gaussian_sigma = 100.0;


long n_times = 100000;




void do_sampling(at_t *at1, at_t *at2, long n_times, long t0)
{
  at_llong_t t;
  at_distr_domain_t *domain = at1->mb->distr->domain;

  // This RNG is configuration sampling
  // and is independent of that for the Langevin equation.
  zcom_mtrng_t rng[1];

  zcom_mtrng__init_from_seed(rng, t0);

  for (t = t0; t < t0+n_times; t++) {

    double beta = domain->beta_min + zcom_mtrng__rand01(rng) * (domain->beta_max - domain->beta_min);

    /* for the Gaussian energy model
     * Ec = - sigma^2 beta
     * and the energy fluctuation is sigma
     */
    double epot_c = -gaussian_sigma*gaussian_sigma * beta;
    double epot = epot_c + gaussian_sigma * zcom_mtrng__rand_gauss(rng);

    at__step(at1, epot, t, NULL);

    if (at2 != NULL) {
      at__step(at2, epot, t, NULL);
    }

    if (t % 10000 == 0) {
      printf("%g%%%20s\r", 100.*t/n_times, "");
    }
  }

  printf("%30s\n", "");
}



// this function does everything at__output() does
void save_data(at_t *at, const char *data_dir)
{
  if (data_dir != NULL) {
    zcom_ssm_t *ssm = at->utils->ssm;

    at->file_binary = at_utils__make_output_filename(ssm, data_dir, "at.dat");
    at->file_text = at_utils__make_output_filename(ssm, data_dir, "at-text.dat");

    at->mb->file_binary = at_utils__make_output_filename(ssm, data_dir, "mb.dat");
    at->mb->file_text = at_utils__make_output_filename(ssm, data_dir, "mb-text.dat");
    at->mb->ze_file = at_utils__make_output_filename(ssm, data_dir, "ze.dat");

    at->driver->langevin->file = at_utils__make_output_filename(ssm, data_dir, "langevin.dat");
    at->driver->langevin->rng->file = at_utils__make_output_filename(ssm, data_dir, "langevin-rng.dat");

    at->eh->file_binary = at_utils__make_output_filename(ssm, data_dir, "ehist.dat");
    at->eh->file_text = at_utils__make_output_filename(ssm, data_dir, "ehist-text.dat");
    at->eh->recon->file = at_utils__make_output_filename(ssm, data_dir, "ehist-recon.dat");
  }

  at__write_self(at);

  at_mb__write(at->mb, at->write_text_file);
  //at_mb__write_ze_file(at->mb, NULL);

  at_driver__write(at->driver);

  at_eh__write(at->eh, at->write_text_file);
  //at_eh__reconstruct(at->eh, NULL);
}



// this function does everything at__output() does
void load_data(at_t *at)
{
  at__load_data(at);
}


at_bool_t compare_files(const char *dir1, const char *dir2,
    const char *fn)
{
  char cmd[1024];
  int ret;
  at_bool_t passed;

  sprintf(cmd, "cmp %s/%s %s/%s", dir1, fn, dir2, fn);
  ret = system(cmd);
  fprintf(stderr, "cmp %s returns %d\n", fn, ret);
  passed = (ret == 0);

  return passed;
}


at_bool_t compare_results(zcom_ssm_t *ssm,
    const char *dir1, const char *dir2)
{

  if (!compare_files(dir1, dir2, "at.dat")) {
    return AT__FALSE;
  }

  if (!compare_files(dir1, dir2, "mb.dat")) {
    return AT__FALSE;
  }

  if (!compare_files(dir1, dir2, "langevin.dat")) {
    return AT__FALSE;
  }

  if (!compare_files(dir1, dir2, "langevin-rng.dat")) {
    return AT__FALSE;
  }

  if (!compare_files(dir1, dir2, "ehist.dat")) {
    return AT__FALSE;
  }

  return AT__TRUE;
}

int do_test(const char *fn_cfg)
{
  at_t at1[1], at2[1];
  at_bool_t passed = AT__TRUE;

  // 1. init the first at object
  at_params_sys_t sys_params1[1];

  sys_params1->boltz = boltz;
  sys_params1->is_continuation = AT__FALSE;
  sys_params1->md_time_step = 0.002;
  sys_params1->sim_id = 0;
  sys_params1->multi_sims = AT__FALSE;

  if (at__init(at1, fn_cfg, sys_params1, AT__INIT_VERBOSE) != 0) {
    return -1;
  }

  at__manifest(at1);

  // 2. normal sampling for at1 for n_times
  do_sampling(at1, NULL, n_times, 0);

  // 3. at1 saves its data
  save_data(at1, NULL);

  fprintf(stderr, "First part is done!\n");

  // 4. init the second at object
  at_params_sys_t sys_params2[1];

  sys_params2->boltz = boltz;
  sys_params2->is_continuation = AT__TRUE;
  sys_params2->md_time_step = 0.002;
  sys_params2->sim_id = 0;
  sys_params2->multi_sims = AT__FALSE;

  if (at__init(at2, fn_cfg, sys_params2, AT__INIT_VERBOSE|AT__INIT_IGNORE_LOCKFILE) != 0) {
    return -1;
  }

  fprintf(stderr, "at->beta %g vs %g\n", at1->beta, at2->beta);
  //exit(1); getchar();

  // 5. at1 and at2 both continue sampling for another n_times
  do_sampling(at1, at2, n_times, n_times);

  // 6. at1 and at2 save data to different directories
  save_data(at1, "atdata1");
  save_data(at2, "atdata2");

  // 7. compare the final results from at1 and at2
  passed = compare_results(at1->utils->ssm, "atdata1", "atdata2");

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  at__finish(at1);
  at__finish(at2);

  return 0;
}



int main(int argc, char **argv)
{
  const char *fn_cfg = "at.cfg";

  if (argc > 1) {
    n_times = atol(argv[1]);
  }

  do_test(fn_cfg);

  return 0;
}
