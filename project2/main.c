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

#include "tools.h"

int main(int argc, char ** argv) {
    
    struct threadParams * params = (struct threadParams *) malloc(sizeof(struct threadParams));
    params->header = "movie_title";
    params->isNumeric = 0;
    params->output = "out";
    params->sortIndex = 11;
    sprintf(params->path, "in");
    
    pthread_t kid;
    
    pthread_create(&kid, NULL, processCsvDir, params);
    
    pthread_join(kid, NULL);
    
    printf("done\n");

    
    return 0;
}
