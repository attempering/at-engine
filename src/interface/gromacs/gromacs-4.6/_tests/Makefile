CC = gcc -O0 -g -Wall -Wno-unused-variable -Wno-unused-function -Wno-stringop-truncation
LM = -lm

bins: use_adaptive_tempering_driver.exe

use_adaptive_tempering_driver.exe : use_adaptive_tempering_driver.c
	$(CC) -I../include -Igmxmock/include -I../src/kernel $< ../src/kernel/adaptive_tempering_driver.c gmxmock/src/gmxmock.c -o $@ $(LM)

clean::
	$(RM) use_adaptive_tempering_driver.exe .*.swp *~ hist*.bin at.manifest* HMB* MTSEED* mb*.av ZE* ze.dat *_hist*.dat TRACE* log.dat cst.dat gmon.out data/*

