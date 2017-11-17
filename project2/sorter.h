#ifndef sorter_h
#define sorter_h

#include "tools.h"

extern pthread_mutex_t m;
extern struct table * * tables;
extern unsigned int tc;

struct mergeTablesParams {
    
    struct table ** tables;
    unsigned int numTables;
    unsigned int sortIndex;
    int isNumeric;
};

void * sortCsv(void * threadParams);
void * mergeTables(void * parameters);

#endif /* sorter_h */
