# Core adaptive tempering code

## Overview

The refactored adaptive tempering code.

* The [at](at) module contains the basic module.

## Modules

* [at](at) consists of the basic functionalities.
  This module is
  * self-contained, can perform basic simulated tempering simulation.
  * portable, written in ANSI C
  * functions are named in snake case convention

* [zcom](zcom): common utilities

### Design principles

* Kernel files have been refactored in favor of a modular design.

* Kernel files comprise a header-only library written in C.
  The `xxx.h` files are the declarations,
  the `xxx__src.h` files are implementations.
  The latter can been included as header files,
  or compiled as regular `.c` files

* Kernel files can be compiled with an ANSI C compiler
  without dependencies on external libraries.
  They can also be compiled along with the rest of the GROMACS
  code.

## New features

Compared with the 2010 paper, the current code brings about
a few new features.

* Metropolisation.

    The Langevin equation used in the original
    algorithm is approximate, and may be inaccurate for an overly
    large step size. This limitation can be removed with the
    Metropolisation technique, which gives an acceptance probability
    for the destination temperature suggested by the Langevin equation.

    Module: [at/driver/langevin/move](at/driver/langevin/move).

* Moderation on the temperature diffusion.

    Initial stages of the
    simulation often pose stability problems because of lack of data to
    guide an efficient diffusion in the temperature space.  A few
    parameters `langevin_no_skip`, `langevin_bin_min_visits`
    are now added to prevent unreasonably large temperature transitions
    in early stages.

    Module: [at/driver/langevin/move](at/driver/langevin/move).

* Zero-filler for the integral identity.

    The integral identity draws estimates from
    a window of bins. Sometimes the bin range contains missing data
    that needs to be filled in order for the identity to work proper.
    The zero-filling mechanism is now added.

    Module: [at/mb/iie/zerofiller](at/mb/iie/zerofiller).

* Caching of the estimated bin-averaged energy (not quite useful).

    A caching mechanism is introduced to save recently computed results
    of the bin-averaged energy which is computed from an integral
    identity. The computation for the estimation can be a bit
    expensive, and the caching mechanism aims at alleviating the issue.
    However, it currently offer little improvement as the cost of
    adding data points largely outweighs the cost of invoking
    the integral identity.

    Module: [at/mb/iie/et/at_mb_iie_et_cache__src.h](at/mb/iie/et/at_mb_iie_et_cache__src.h).
