# Kernel code for adaptive tempering

## Overview

The adaptive tempering algorithm is a single-copy simulated tempering
algorithm that is based on a continuous temperature range.
The algorithm adopts a few self-learning techniques
to adaptive the parameters related to statistical weights
(the partition function).

The current code is written in ANSI C,
its and is compatible with GROMACS 4.6.

## Getting started

A typical simulation goes like this:

```C
at_t* at = at__open("at.cfg", NULL, AT__INIT_VERBOSE);

for (step = 1; step <= nsteps; step++) {
  at->energy = 0.0;
  at__move(at, step, NULL);
}

at__close(at);
```

Simulation parameters are provided in the configuration file `at.cfg`.

## Options

A brief manual on the configuration file `at.cfg`
is included under the [doc](doc) directory.

## Improvements

Compared with the 2010 paper, the current code brings about
a few key improvements on the overall accuracy and performance.

* Metropolisation.  The Langevin equation used in the original
algorithm is approximate, and may be inaccurate for an overly
large step size. This limitation can be removed with the
Metropolisation technique, which gives an acceptance probability
for the destination temperature suggested by the Langevin equation.

* Moderation on the temperature diffusion.  Initial stages of the
simulation often pose stability problems because of lack of data to
guide an efficient diffusion in the temperature space.  A few
parameters `langevin_no_skip`, `langevin_bin_min_visits`
are now added to prevent unreasonably large temperature transitions
in early stages.

* Caching of the estimated bin-averaged energy.  A caching
mechanism is introduced to save recently computed results
of the bin-averaged energy which is computed from an integral
identity. The computation for the estimation can be a bit
expensive, and the caching mechanism aims at alleviating the issue.

* Zero-filler. The integral identity draws estimates from
a window of bins. Sometimes the bin range contains missing data
that needs to be filled in order for the identity to work proper.
The zero-filling mechanism is now added.

## For developers

[Test case: Gaussian energy model](src/sample_progs/gaussian)

[GROMACS plugin](src/interface/gromacs/4.6/src/kernel/adaptive_tempering)

The adaptive tempering algorithm is the main engine that drives
the temperature transitions in the multiple-canonical ensemble.

### Things to know

* Kernel files have been refactored in favor of a modular design.

* Kernel files is a header-only library written in C.
  The `xxx.h` files are the declarations,
  the `xxx__src.h` files are implementations.
  The latter can been included as header files,
  or compiled as `.c` files

* Kernel files can be compiled with an ANSI C compiler
  without dependencies on external libraries.
  They can also be compiled along with the rest of the GROMACS
  code.

* Sample test cases using the API offered by the kernel files
  are collected in the [sample_progs](sample_progs) directory.
