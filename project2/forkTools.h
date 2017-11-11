#ifndef forkTools_h
#define forkTools_h

#define TEMPSIZE 4096

#include "memTools.h"

// Represents a sorted CSV file.
struct csv {
    
    char * path;
    char * errors;
    char sorted;
    char error;
};

// Represents a processed directory.
struct csvDir {
    
    char * path;
    pid_t * subDirsPids;
    pid_t * csvPids;
    unsigned int numSubDirs;
    unsigned int numCsvs;
};

void processCsvDir(const char * path, struct sharedMem * sharedMem,
                   const char * columnHeaders, const char * outputDir);

#endif /* forkTools_h */
