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
#include <semaphore.h>

#include "sorter.h"

int main(int argc, char ** argv) {
    
    IsNumeric = 0;
    SortIndex = 11;
    Header = "movie_title";
    OutputDir = "out";
    
    struct threadParams * params = (struct threadParams *) malloc(sizeof(struct threadParams));
    sprintf(params->path, "in");
    
    
    pthread_t kid;
    pthread_create(&kid, NULL, processCsvDir, params);

    
    pthread_mutex_lock(&DSM);
    while (!DoneSorting) {
        pthread_cond_wait(&DSCV, &DSM);
    }
    pthread_mutex_unlock(&DSM);
    
    
    printf("done %d\n", CsvCounter);

    
    return 0;
}
