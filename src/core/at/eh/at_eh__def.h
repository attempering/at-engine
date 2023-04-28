#ifndef AT_EH__DEF_H__
#define AT_EH__DEF_H__

#include "../mb/at_mb__def.h"

typedef struct at_eh_t_
{
  int        n;         /* number of temperature bins */
  int        mode;      /* 0: disable; 1: simple histogram */
  int        skip;      /* interval of reconstructing energy histograms */
  int        bwmod;     /* 0: d(beta) 1: dT/T  2: d(kT) */
  double     bwdel;     /* delta lnT */
  double     min;       /* minimal energy */
  double     max;       /* maximal energy */
  double     del;       /* energy bin size */
  int        cnt;       /* number of energy bins */
  int        binary;    /* use binary format for IO */
  int        nst_save;   /* interval of writing histogram files */
  char      *file;      /* name of IO file */
  char      *rfile;     /* name of reconstructed energy histogram */
  double    *his;       /* energy histogram data */
  double    *recon;     /* temporary space for reconstructing histogram */
  int       *is;        /* indices for temperature windows (lower) */
  int       *it;        /* indices for temperature windows (higher) */
  unsigned   flags;
  at_mb_t      *mb;        /* associated at_mb_t object */
} at_eh_t;

/* IO flags */
#define   AT_EH_ADDAHALF    0x00010000    /* add a half energy bin width in output */
#define   AT_EH_KEEPEDGE    0x00020000    /* keep zero edge at sides */
#define   AT_EH_NOZEROES    0x00040000    /* do not output zeroes */

#endif
