#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "sorter.h"
#include "tools.h"

int main(int argc, char ** argv) {
    
    IsNumeric = 0;
    SortIndex = 11;
    Header = "movie_title";
    OutputDir = "out";
    
    char * path = malloc(3);
    sprintf(path, "in");
    
    
    pthread_t kid;
    pthread_create(&kid, NULL, processCsvDir, path);

    
    pthread_mutex_lock(&DSM);
    while (!DoneSorting) {
        pthread_cond_wait(&DSCV, &DSM);
    }
    pthread_mutex_unlock(&DSM);
    
    
    printf("done %d\n", CsvCounter);

    
    return 0;
}
