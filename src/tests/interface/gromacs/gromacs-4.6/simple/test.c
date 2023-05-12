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

#include "at-gromacs/at-gromacs__src.h"



void init_gromacs_vars(t_commrec *cr, t_inputrec *ir, gmx_enerdata_t *enerd)
{
  int world_size = 1, world_rank = 0;

#ifdef GMX_MPI
  cr->mpi_comm_mygroup = MPI_COMM_WORLD;
  cr->mpi_comm_mysim = MPI_COMM_WORLD;

  // Get the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  fprintf(stderr, "node %d/%d\n", world_rank, world_size);
#endif

  cr->nodeid = world_rank;
  cr->nnodes = world_size;

  cr->ms = NULL;

  ir->delta_t = 0.002;

  ir->opts.ngtc = 1;
  ir->opts.ref_t = calloc(1, sizeof(real));
  ir->opts.ref_t[0] = 300;

  enerd->term[F_EPOT] = 0.0;
}


int work(int argc, char **argv)
{
  at_llong_t step = 0, nsteps = 10;
  atgmx_t atgmx[1];
  t_commrec cr[1];
  t_inputrec ir[1];
  gmx_enerdata_t enerd[1];
  at_bool_t from_cpt = AT__FALSE;

  // initialize GROMACS variables
  init_gromacs_vars(cr, ir, enerd);

  atgmx__init(atgmx, "at.cfg", ir, cr, from_cpt, AT__INIT_VERBOSE);

  for (step = 1; step <= nsteps; step++) {
    at_bool_t is_first_step = (step == 1);
    at_bool_t is_last_step = (step == nsteps);
    at_bool_t has_global_stats = AT__TRUE;
    at_bool_t is_xtc_step = AT__TRUE;
    at_bool_t is_ns_step = AT__TRUE;

    enerd->term[F_EPOT] = -10000.0;

    atgmx__move(atgmx, enerd, step,
        is_first_step, is_last_step, has_global_stats,
        is_xtc_step, is_ns_step,
        cr);

    if (MASTER(cr)) {
      printf("step %ld: beta %g\n", (long) step, atgmx->at->beta);
    }
  }

  atgmx__finish(atgmx);

  free(ir->opts.ref_t);

#ifdef GMX_MPI
  MPI_Finalize();
#endif

  return 0;
}


int main(int argc, char **argv)
{

#ifdef GMX_LIB_MPI
  MPI_Init(&argc, &argv);
#endif

#ifdef GMX_THREAD_MPI
  tMPI_Init(&argc, &argv, &work);
#endif

  work(argc, argv);

  return 0;
}
