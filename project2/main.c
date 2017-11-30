#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "sorter.h"
#include "tools.h"

int main(int argc, char ** argv) {
    
    printf("Initial PID: %d\n\tTIDS of all child threads: ", getpid());
    
    CsvErrors = malloc(TEMPSIZE * TEMPSIZE);
    char * csvErrors = CsvErrors;

    IsNumeric = 0;
    SortIndex = 11;
    Header = "movie_title";
    OutputDir = "out";
    
    char * path = malloc(TEMPSIZE);
    sprintf(path, "in");
    
    
    pthread_t kid;
    pthread_create(&kid, NULL, processCsvDir, path);
    
    printf("%lu,", (unsigned long) kid);
    
    pthread_mutex_lock(&DSM);
    while (!DoneSorting) {
        pthread_cond_wait(&DSCV, &DSM);
    }
    pthread_mutex_unlock(&DSM);
    
    printf("\n\tTotal number of threads: %u\n", AllThreadsCount);
    fprintf(stderr,"\n%s\n", csvErrors);
    printf("Done\n");
    
    free(csvErrors);
    
    return 0;
}
