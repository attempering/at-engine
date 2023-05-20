# Interface to GROMACS 5.1

This directory contains files relevant to using at-engine in GROMACS.

## Directory structure

* [gmxroot](gmxroot) symbolic link to the external modified source code tree.
* [gmxmock](gmxmock) mock files to GROMACS files so that test programs can be compiled without the actual GROMACS code.
* [modified](modified) modified files, [list of modifications](modified/README.md)

## Building GROMACS

### Regular a CPU version

```sh
mkdir build
cd build
cmake .. -DGMX_GPU=OFF -DGMX_BUILD_OWN_FFTW=ON
make -j 8
```

### Building MPI version

First install OpenMPI or an alternative

```sh
mkdir build_mpi
cd build_mpi
cmake .. -DGMX_GPU=OFF -DGMX_MPI=ON -DGMX_BUILD_OWN_FFTW=ON
make
```