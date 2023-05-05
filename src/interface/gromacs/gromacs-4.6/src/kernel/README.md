# GROMACS wrapper directory

## Overview

This directory provides a wrapper for
the continuous-temperature simulated tempering code.
The code name is, however, 'adaptive tempering',
to be consistent with the legacy code.

[atgmx_driver.c](adaptive_tempering_driver.c)
and
[atgmx_driver.h](adaptive_tempering_driver.h)
provide the source code and header of module
in the traditional sense.

## Usage

This directory, `src/kernel`, coincides with the GROMACS 4.6 directory of the same path.

When using this adaptive-tempering module in GROMACS 4.6,
one can simply copy or link the following files and directories
into the corresponding `src/kernel` directory of GROMACS.

* [adaptive_tempering_core](adaptive_tempering_core)
* [adaptive_tempering_driver.h](adaptive_tempering_driver.h)
* [adaptive_tempering_driver.c](adaptive_tempering_driver.c)

## Notes

The `adaptive_tempering` module provides functionalities
for a basic simulated tempering simulation.
The multiple-topology code is not included in this module.
