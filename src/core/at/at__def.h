#ifndef AT__AT__DEF_H__
#define AT__AT__DEF_H__



/* this header needs to be placed before zcom modules
 * so that we can determine whether `real` should be defined or not */
#include "context/context.h"



#include "zcom/def/def.h"

#include "mb/mb.h"
#include "eh/eh.h"
#include "langevin/langevin.h"



/* define a long long int type */
#ifndef I32ONLY
typedef long long  llong_t;
#define llong_pfmt "%lld"
#else
typedef long llong_t;
#define llong_pfmt "%ld"
#endif



typedef struct at_logfile_t_ at_logfile_t;
typedef struct zcom_mtrng_t_ zcom_mtrng_t;



typedef struct at_t_ {
  double    boltz;            // Boltzmann constant

  double    beta;             // current beta

  double    temp_thermostat;  // thermostat temperature

  double    md_time_step;     // MD integration step for reference
  int       nsttemp;          // interval of tempering, 0: disable, -1: only when doing neighbor searching
  int       nst_log;          // interval of writing trace file, 0: disable, -1: only when doing neighbor searching

  double    energy;           // current coupling energy

  /* high-temperature bias

    H = kappa* H0 + epsilon * H1
    kappa = 1-(T-Tref)*(1-kappa0)/(Tmax-Tref) if T>Tref; kappa=1 if T<Tref
    epsilon= epsilon0*(T-Tref)/(Tmax-Tref) if T>Tref; epsilon=0 if T<Tref

  */

  at_bool_t bTH;
  double    TH_Tref;
  double    *kappa, *epsilon;
  double    kappa0, epsilon0;

  at_mb_t       mb[1];        // multiple-bin estimator

  at_langevin_t langevin[1];  // Langevin equation

  at_eh_t       eh[1];        // energy histogram

  zcom_ssm_t    *ssm;         // string allocator

  zcom_mtrng_t  *mtrng;       // random number generator

  char          *rng_file;    // random number file name
  char          *log_file;    // log/trace file name
  at_logfile_t  *log;         // log file
  char          ch_suffix;    // file suffix as a single-digit character
  char          data_dir[32];

  int mpi_rank, mpi_size;

#ifdef GMX_MPI
  MPI_Comm mpi_comm;
#endif

} at_t;


#endif
