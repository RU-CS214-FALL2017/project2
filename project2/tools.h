#ifndef tools_h
#define tools_h

#include <stdio.h>
#include <sys/types.h>

#include "forkTools.h"

unsigned int tokenizeRow(const char * line, char * ** row);
void trim (char * str);
void removeChars (char * str, unsigned long startIndex, unsigned long endIndex);
void fillTable(FILE * csvFile, char * *** table, unsigned int * rows, unsigned int * columns);
void printTable (FILE * stream, char *** table, unsigned int rows, unsigned int columns);
int isNumber(const char * str);
int isXBeforeY (const char * x, const char * y, int areNumbers);
int isNumericColumn(char *** table, int rows, int columnIndex);
int findCsvFiles(const char * dirPath, char * ** csvPaths, int * numFound);
int isCsv(const char * csvPath);
unsigned int lineageParser(const char * path, char * ** lineage);
char * sortedCsvPath(const char * csvPath, const char * columnHeader, const char * outputDir);
int getColumnHeaderIndex(const char * columnHeader,
                         char *** table, const unsigned int columns);
void printDirTree(FILE * output, struct sharedMem * sharedMem);
unsigned int dirSubProcessCount(pid_t dirPid, struct sharedMem * sharedMem);
void checkDir(const char * path, const char * dirType);

#endif /* tools_h */
