#ifndef tools_h
#define tools_h

#define TEMPSIZE 4096
#define COLUMNS 28

#include <stdio.h>
#include <sys/types.h>

struct table {
  
    char *** table;
    char * ** rowsMems;
    char * * cellsMems;
    unsigned int numRows;
    unsigned int numRowsMems;
    unsigned int numCellsMems;
};

extern const char * MovieHeaders[28];

int tokenizeRow(char * line, char ** row, int isHeader);
void removeTrail(char * str);
void trim (char * str);
void removeChars (char * str, unsigned long startIndex, unsigned long endIndex);
int fillTable(const char * csvPath, struct table * table);
void printTable (FILE * stream, char *** table, unsigned int rows);
int isNumber(const char * str);
int isXBeforeY (const char * x, const char * y);
int isNumericColumn(char *** table, int rows, int columnIndex);
int findCsvFiles(const char * dirPath, char * ** csvPaths, int * numFound);
int isCsv(const char * csvPath);
unsigned int lineageParser(const char * path, char * ** lineage);
void printToSortedCsvPath(const char * csvPath, char *** table, unsigned int rows);
int getColumnHeaderIndex(const char * columnHeader,
                         char *** table, const unsigned int columns);
void checkDir(const char * path, const char * dirType);
unsigned int getIndex(const char * header, int * isNumeric);
void * processCsvDir(void * threadParams);

#endif /* tools_h */
