//#include <string.h>
//#include <stdlib.h>
//#include <dirent.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include <fcntl.h>
//#include <sys/mman.h>
//#include <sys/stat.h>
//#include <errno.h>
//#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "sorter.h"

int main(int argc, char ** argv) {
    
    struct threadParams * params = (struct threadParams *) malloc(sizeof(struct threadParams));
    params->header = "movie_title";
    params->isNumeric = 0;
    params->output = "out";
    params->sortIndex = 11;
    sprintf(params->path, "in");
    
    pthread_mutex_init(&m, NULL);
    tables = malloc(sizeof(struct table *) * TEMPSIZE);
    
    pthread_t kid;
    pthread_create(&kid, NULL, processCsvDir, params);
    
    struct mergeTablesParams * params2 = malloc(sizeof(struct mergeTablesParams));
    params2->isNumeric = 0;
    params2->numTables = tc;
    params2->sortIndex = 11;
    params2->tables = tables;
    
    pthread_t kid2;
    void * ret;
    
    pthread_join(kid, NULL);
    
    pthread_create(&kid2, NULL, mergeTables, params2);
    pthread_join(kid2, &ret);
    struct table * retTable = ret;
    
    printToSortedCsvPath("in/all.csv", "movie_title", "out", retTable->table, retTable->numRows);
    
    
    printf("done\n");

    
    return 0;
}
