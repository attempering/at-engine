#include "at-gromacs/atgmx/context/atgmx_context.h"

t_inputrec::t_inputrec() {
  opts.ref_t = nullptr;
}

t_inputrec::~t_inputrec() {
  free(opts.ref_t);
}

ForeignLambdaTerms::ForeignLambdaTerms(int numLambdas) :
    numLambdas_(numLambdas),
    energies_(1 + numLambdas),
    dhdl_(1 + numLambdas)
{
}

gmx_enerdata_t::gmx_enerdata_t(int numEnergyGroups, int numFepLambdas) :
    grpp(numEnergyGroups),
    foreignLambdaTerms(numFepLambdas),
    foreign_grpp(numEnergyGroups)
{
}
