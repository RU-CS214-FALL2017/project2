#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "tools.h"
#include "sorter.h"
#include "mainTools.h"
#include "forkTools.h"
#include "memTools.h"

int main(int argc, char ** argv) {

    char * columnHeaders = getColumnHeader(argc, argv);
    char * inputDirecory = getInputDirectory(argc, argv);

    if (inputDirecory == NULL) {
        inputDirecory = ".";
    }

    checkDir(inputDirecory, "input");

    char * outputDirectory = getOutputDirectory(argc, argv);
    if (outputDirectory != NULL) {
        checkDir(outputDirectory, "output");
    }

    printf("Initial PID: %d\n", getpid());
    printf("PIDS of all child processes: ");
    fflush(stdout);

    struct sharedMem * sharedMem = initSharedMem();
    
    processCsvDir(inputDirecory, sharedMem, columnHeaders, outputDirectory);

    printf("\nTotal number of processes: %u\n\n", dirSubProcessCount(getpid(), sharedMem));
    
    printDirTree(stdout, sharedMem);
    FILE * output = fopen("structure.txt", "w");
    printDirTree(output, sharedMem);
    fclose(output);
    
    printf("\nAlso saved the structure to structure.txt\n");
    
    freeSharedMem(sharedMem);

    exit(EXIT_SUCCESS);
}
