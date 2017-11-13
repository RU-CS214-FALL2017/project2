flags = -std=gnu11 -Wall
dir = project2
sourceFiles = $(dir)/main.c $(dir)/sorter.c $(dir)/tools.c $(dir)/mainTools.c $(dir)/memTools.c $(dir)/forkTools.c
allFiles = $(sourceFiles) $(dir)/sorter.h $(dir)/tools.h $(dir)/mainTools.h $(dir)/memTools.h $(dir)/forkTools.h

sorter: $(allFiles)
	gcc $(flags) $(sourceFiles) -o sorter

debug: $(allFiles)
	gcc $(flags) -g $(sourceFiles) -o debugSorter