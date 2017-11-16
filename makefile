flags = -std=gnu11 -Wall -pthread
dir = project2
sourceFiles = $(dir)/main.c $(dir)/sorter.c $(dir)/tools.c $(dir)/mainTools.c $(dir)/memTools.c $(dir)/forkTools.c
allFiles = $(sourceFiles) $(dir)/sorter.h $(dir)/tools.h $(dir)/mainTools.h $(dir)/memTools.h $(dir)/forkTools.h
nvcc = /usr/local/cuda/bin/nvcc

sorter: $(allFiles)
	gcc $(flags) $(sourceFiles) -o sorter

gpusorter: $(allFiles)
	$(nvcc) -Xcompiler -std=gnu11,-Wall,-pthread $(sourceFiles) -o gpusorter

debug: $(allFiles)
	gcc $(flags) -g $(sourceFiles) -o debugsorter