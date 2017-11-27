#ifndef sorter_h
#define sorter_h

#include "tools.h"

extern int CsvCounter;
extern pthread_mutex_t CCM;

extern sem_t S;

extern char * Header;
extern unsigned int SortIndex;
extern int IsNumeric;
extern char * OutputDir;

struct mergeTablesParams {
    
    struct table ** tables;
    unsigned int numTables;
    unsigned int sortIndex;
    int isNumeric;
};

void increment(void);
void decrement(void);
void * sortCsv(void * threadParams);
void mergeTables(struct table * table);
//struct table * mergeTables(struct table ** tables, unsigned int numTables, unsigned int sortIndex, int isNumeric);
//void * mergeTablesHelper(void * parameters);

#endif /* sorter_h */
