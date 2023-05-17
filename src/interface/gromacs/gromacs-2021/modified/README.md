# Modified files to the GROMACS source code

## List of main modifications

### Symbolically linking the at-gromacs directory under `src/gromacs`

```sh
cd src/gromacs
ln -sf ../../../../at-engine/src/interface/gromacs/gromacs-2021/modified/src/gromacs/at-gromacs
```

Make sure there is a `CMakeLists.txt` under the `at-gromacs` directory,
even if it is an empty file.

Editing the `CMakeLists.txt` under the `gromacs`.

Adding the following piece of code:

```cmake
######## Beginning of modifications for at-gromacs #######

add_subdirectory(at-gromacs)

# force rebuild `at-gromacs__src.h` every time
add_custom_target(
  update_at_gromacs
  ALL
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/at-gromacs/at-gromacs__src.h
)

# add the target `update_at_gromacs` as a dependency of libgromacs
add_dependencies(libgromacs update_at_gromacs)

######## End of modifications for at-gromacs #######
```

A possible position is after on line 336:

```cmake
target_link_libraries(libgromacs PRIVATE lmfit)
```

### Adding a file type for `.cfg` files

1. Add an enum `cfCFG` before `efNR` in [include/types/filenm.h](include/types/filenm.h).

2. Add the following line to [src/gmxlib/filenm.c](src/gmxlib/filenm.c)
   at the end of the definition of `t_deffile`:

    ```C
    { eftASC, ".cfg", "at", NULL, "AT-Engine configuration file" }
    ```

    Remember to add a comma at the end of the previous line.

3. Add the following line to the definition of variable `fnm` in [src/kernel/mdrun.c](src/kernel/mdrun.c)

    ```C
    { cfCFG, "-at",  NULL, ffOPTRD },
    ```

    Remember to add a comma at the end of the previous line.

### at-gromacs code to `src/gromacs/mdrun/md.cpp`

1. Adding a `#include` statement to `src/gromacs/mdrun/md.cpp`

    ```C++
    #include "gromacs/at-gromacs/at-gromacs__src.h"
    ```

Modify the function `do_md()` in [src/kernel/md.c](src/kernel/md.c)

1. At the beginning of the function, before entering the MD loop, add

    ```C
    atgmx_t atgmx[1];

    atgmx__init(atgmx,
        atgmx__opt2fn("-at", nfile, fnm),
        ir, cr,
        Flags & MD_STARTFROMCPT,
        AT__INIT_VERBOSE);
    ```

2. Within the MD loop, right before the line (about line 1154) of assignment `bGStat = ...`, add

    ```C
        if (atgmx__do_tempering_on_step(atgmx, step, bNS)) {
            bCalcEner = TRUE;
            bCalcVir = TRUE;
        }
    ```

3. Still in the MD loop, after the call of `do_force()` and the call of `GMX_BARRIER(cr->mpi_comm_mygroup)`, add

    ```C
    atgmx__scale_force(atgmx, f, mdatoms);
    ```

4. Before the comment of `START TRAJECTORY OUTPUT`

    ```C
        atgmx__move(atgmx, enerd, step, bFirstStep, bLastStep,
            bGStat, do_per_step(step, ir->nstxtcout), bNS, cr);
    ```

5. At the end of function

    ```C
    atgmx__finish(atgmx);
    ```

## Script to sync modified files

```sh
./sync_mods.sh
```

Modified files are hard linked.
