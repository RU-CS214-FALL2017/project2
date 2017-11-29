#ifndef sorter_h
#define sorter_h

#include "tools.h"

extern int CsvCounter;
extern pthread_mutex_t CCM;
extern pthread_mutex_t QM;

extern int DoneSorting;
extern pthread_cond_t DSCV;
extern pthread_mutex_t DSM;

extern char * Header;
extern unsigned int SortIndex;
extern int IsNumeric;
extern char * OutputDir;

void increment(void);
void decrement(void);
void * sortCsv(void * threadParams);
void mergeThreads(struct table * table);
void mergeTables(struct table * table1, struct table * table2);

#endif /* sorter_h */
