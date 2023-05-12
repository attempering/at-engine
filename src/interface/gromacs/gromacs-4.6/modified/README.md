# Modified files to the GROMACS source code

## List of main modifications

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

### atgmx code to src/kernel/md.c

## Script to sync modified files

```sh
./sync_mods.sh
```

Modified files are hard linked.
