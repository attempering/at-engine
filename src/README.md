# Refactored adaptive tempering code in C

## Modules

* [core](core) kernel code for adaptive tempering.

* [external](external) veritools.

* [interface](interface) interface to GROMACS.

* [unit_tests](unit_tests) unit test programs.

* [sample_progs](sample_progs) sample programs.

## Test programs

Many test programs have been built into the project.

Normally, test in this order:

1. [Basic compiling tests to the core files](core/_tests)
2. [Unit tests to the modules of the core files](unit_tests)
3. [Sample programs](sample_progs)
4. [Basic tests to the GROMACS 4.6 plugins](interface/gromacs/4.6/_tests)
