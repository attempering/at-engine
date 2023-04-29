CC = gcc -O3 -g -Wall -Wno-unused-function -Wno-stringop-truncation
CC_DBG = gcc -O0 -g -pg -Wall -Wno-unused-function -Wno-unused-variable -Wno-stringop-truncation
LM = -lm

srcs = $(wildcard *.c)

header_only_bins = $(patsubst %.c, %_header_only.exe, $(srcs))
header_source_bins = $(patsubst %.c, %_header_source.exe, $(srcs))

includes = -I../../../core

all: $(header_only_bins) $(header_source_bins)

use_at_header_only.exe : use_at.c
	$(CC) -DHEADER_ONLY=1 $(includes) $< -o $@ $(LM)

use_at_header_source.exe : use_at.c
	$(CC) $(includes) $< ../../../core/at/at.c -o $@ $(LM)

clean::
	$(RM) $(header_only_bins) $(header_source_bins) .*.swp *~ hist*.bin at.manifest* HMB* MTSEED* mb*.av ZE* ze.dat *_hist*.dat TRACE* log.dat cst.dat gmon.out data/*
