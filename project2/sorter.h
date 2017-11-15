#ifndef sorter_h
#define sorter_h

#include "tools.h"

struct mergeTablesParams {
    
    struct table ** tables;
    unsigned int numTables;
    unsigned int sortIndex;
    int isNumeric;
};

void * sortCsv(void * threadParams);
void * mergeTables(void * parameters);

#endif /* sorter_h */
