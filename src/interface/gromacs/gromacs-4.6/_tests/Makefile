CC = gcc -O0 -g -Wall -Wno-unused-variable -Wno-unused-function -Wno-stringop-truncation
LM = -lm

bins: use_at-gromacs.exe

use_at-gromacs.exe : use_at-gromacs.c
	$(CC) -I../include -Igmxmock/include -I../src/kernel $< ../src/kernel/at-gromacs/at-gromacs.c gmxmock/src/gmxmock.c -o $@ $(LM)

clean::
	$(RM) -rf use_at-gromacs.exe .*.swp *~ hist*.bin at.manifest* HMB* MTSEED* mb*.av ZE* ze.dat *_hist*.dat TRACE* log.dat cst.dat gmon.out data/* atdata*

