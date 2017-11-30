#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "sorter.h"
#include "tools.h"
#include "mainTools.h"

int main(int argc, char ** argv) {
    
    Header = getColumnHeader(argc, argv);
    char * inputDir = getInputDirectory(argc, argv);
    
    if (inputDir == NULL) {
        
        inputDir = malloc(2);
        sprintf(inputDir, ".");
        
    } else {
        
        checkDir(inputDir, "input");
        char * temp = malloc(strlen(inputDir) + 1);
        strcpy(temp, inputDir);
        inputDir = temp;
    }
    
    OutputDir = getOutputDirectory(argc, argv);
    
    if (OutputDir == NULL) {
        
        OutputDir = malloc(2);
        sprintf(OutputDir, ".");
        
    } else {
        
        checkDir(OutputDir, "output");
        char * temp = malloc(strlen(OutputDir) + 1);
        strcpy(temp, OutputDir);
        OutputDir = temp;
    }
    
    SortIndex = getIndex(Header, &IsNumeric);
    
    printf("Initial PID: %d\n\tTIDS of all child threads: ", getpid());

    CsvErrors = malloc(TEMPSIZE * TEMPSIZE);
    char * csvErrors = CsvErrors;
    
    pthread_t kid;
    pthread_create(&kid, NULL, processCsvDir, inputDir);
    
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
