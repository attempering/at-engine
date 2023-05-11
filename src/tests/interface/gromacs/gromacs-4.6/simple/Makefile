CC = gcc -O3 -g -Wall -Wno-unused-function -Wno-stringop-truncation
CC_DBG = gcc -O0 -g -pg -Wall -Wno-unused-function -Wno-unused-variable -Wno-stringop-truncation
LM = -lm

srcs = $(wildcard *.c)
bins = $(patsubst %.c, %.exe, $(srcs))
bins_dbg = $(patsubst %.c, %_d.exe, $(srcs))
dep_srcs =
deps = $(dep_srcs) \
       $(wildcard *.h) \
			 $(wildcard ../../../../../interface/gromacs/gromacs-4.6/src/kernel/at-gromacs/*.h) \
			 $(wildcard ../../../../../interface/gromacs/gromacs-4.6/src/kernel/at-gromacs/*/*.h) \
			 $(wildcard ../../../../../interface/gromacs/gromacs-4.6/src/kernel/at-gromacs/*/*/*.h) \
       Makefile
includes = -I../../../../../interface/gromacs/gromacs-4.6/src/kernel \
           -I../../../../../interface/gromacs/gromacs-4.6/gmxmock/include \
           -I../../../../../interface/gromacs/gromacs-4.6/include \

bins: $(bins)

$(bins): %.exe : %.c $(deps)
	$(CC) $(includes) $< $(dep_srcs) -o $@ $(LM)

$(bins_dbg): %_d.exe : %.c $(deps)
	$(CC_DBG) $(includes) $< $(dep_srcs) -o $@ $(LM)

clean::
	$(RM) -rf $(bins) $(bins_dbg) .*.swp *~ hist*.bin at.manifest* HMB* MTSEED* mb*.av ZE* ze.dat *_hist*.dat TRACE* trace.dat cst.dat gmon.out data/* atdata*
