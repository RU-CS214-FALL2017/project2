flags = -std=gnu11 -Wall -pthread
dir = project2
sourceFiles = $(dir)/main.c $(dir)/sorter.c $(dir)/tools.c $(dir)/mainTools.c $(dir)/queue.c
allFiles = $(sourceFiles) $(dir)/sorter.h $(dir)/tools.h $(dir)/mainTools.h $(dir)/queue.h

sorter: $(allFiles)
	gcc $(flags) $(sourceFiles) -o sorter

debug: $(allFiles)
	gcc $(flags) -g $(sourceFiles) -o debugsorter