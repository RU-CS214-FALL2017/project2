#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "tools.h"
#include "sorter.h"
#include "memTools.h"

//void sortByHeaders(const char * csvPath, const char * columnHeaders, char *** table,
//                   const unsigned int rows, const unsigned int columns, struct sharedMem * sharedMem);
//void cascadeSort(char *** table, unsigned int rows, unsigned int start,
//                 const unsigned int end, const int * headers, const unsigned int numHeaders);
void mergeSort(char *** table, unsigned int columnIndex, int isNumeric, unsigned int start, unsigned int end);
void merge(char *** table, unsigned int columnIndex, int isNumeric, unsigned int start, unsigned int mid, unsigned int end);

// Sorts a the CSV file at <csvPath> in ascending order on the
// column header <columnHeader> at index <sortIndex>. Saves the
// sorted csv file in <outputDir>.
void * sortCsv(void * threadParams) {
    
    struct threadParams * params = (struct threadParams *) threadParams;
    
    FILE * csv = fopen(params->path, "r");
    
    char *** table = (char ***) malloc(sizeof(char **) * TEMPSIZE * TEMPSIZE);
    char * cells = (char *) malloc(TEMPSIZE * TEMPSIZE);
    
    unsigned int rows = fillTable(csv, table, cells);
    fclose(csv);
    
    mergeSort(table, params->sortIndex, params->isNumeric, 1, rows);
    printToSortedCsvPath(params->path, params->header, params->output, table, rows);
    
    freeTable(table, rows);
    
    pthread_exit(NULL);
}

// Ascendingly sorts <table> with <rows> rows and <columns> columns according to
// the column with the first header in <columnHeaders>. Cascades the sort on the
// other headers in <columnHeaders> in order. <columnHeaders> should be a
// comma-delimited list of headers. Returns 1 if the first header in <columnHeaders>
// was found, else returns 0. Prints errors for cascaded headers not found.
//void sortByIndex(unsigned int sortIndex, char *** table, const unsigned int rows, int isNumeric) {
//
//    ;
//
//    int foundHeaders[numHeaders];
//    foundHeaders[0] = column;
//    int fhi = 1;
//
//    char errors[TEMPSIZE];
//    char * errptr = errors;
//    sprintf(errptr, "Cannot find cascading column headers:");
//    errptr += strlen(errptr);
//
//    for (int i = 1; i < numHeaders; i++) {
//
//        foundHeaders[fhi] = getColumnHeaderIndex(headers[i], table, columns);
//
//        if (foundHeaders[fhi] == -1) {
//
//            csvInfo->error = 1;
//
//            sprintf(errptr, " %s", headers[i]);
//            errptr += strlen(errptr);
//
//        } else {
//            fhi++;
//        }
//    }
//
//    if (csvInfo->error) {
//
//        csvInfo->errors = myalloc(strlen(errors) + 1, sharedMem);
//        strcpy(csvInfo->errors, errors);
//    }
//
//    cascadeSort(table, rows, 1, rows, foundHeaders, fhi);
//    csvInfo->sorted = 1;
//}

//// Cascades the sort of table with <rows> rows according to headers
//// with indexes <headers>. <headers>[0] is the index of the header
//// that <table> is sorted by from <start> to and not including <end>.
//// <numHeaders is the number of elements in <headers>.
//void cascadeSort(char *** table, unsigned int rows, unsigned int start,
//                 const unsigned int end, const int * headers, const unsigned int numHeaders) {
//
//    if (numHeaders > 1) {
//
//        for (unsigned int j = start; j < end; j++) {
//
//            if (strcmp(table[start][headers[0]], table[j][headers[0]])) {
//
//                mergeSort(table, headers[1], isNumericColumn(table, rows, headers[1]), start, j);
//                cascadeSort(table, rows, start, j, &headers[1], numHeaders - 1);
//                start = j;
//            }
//        }
//    }
//}

// Ascendingly sorts <table> according to the column at index <columnIndex> from row at
// index <start> to row at index <end> - 1. If areNumbers is set to 0, the sort
// is done numerically otherwise it is done lexicographically.
void mergeSort(char *** table, unsigned int columnIndex, int isNumeric, unsigned int start, unsigned int end) {
    
    if ((end - start) > 1 ) {
        
        unsigned int mid = ((end - start) / 2) + start;
        
        mergeSort(table, columnIndex, isNumeric, start, mid);
        mergeSort(table, columnIndex, isNumeric, mid, end);
        
        merge(table, columnIndex, isNumeric, start, mid, end);
    }
}

// Merges sub rows from <table>, where subrows1 is from row at index <start>
// inclusive to row at index <mid> exclusive, and subrows2 is from row at
// index <mid> inclusive to row at index <end> exclusive. As the sub rows
// merge, they are sorted ascendingly according to the column at index
// <columnIndex>. If areNumbers is set to 0, the sort is done numerically
// otherwise it is done lexicographically.
void merge(char *** table, unsigned int columnIndex, int isNumeric, unsigned int start, unsigned int mid, unsigned int end) {
    
    char ** temp[end - start];
    unsigned int s = start;
    unsigned int m = mid;
    unsigned int i = 0;

    while(s < mid && m < end) {
        
        if(isXBeforeY((table)[s][columnIndex], (table)[m][columnIndex], isNumeric)) {
            
            temp[i] = (table)[s];
            s++;
            
        } else {
            
            temp[i] = (table)[m];
            m++;
        }
        
        i++;
    }
    
    while (s < mid) {
        
        temp[i] = (table)[s];
        s++;
        i++;
    }
    
    while (m < end) {
        
        temp[i] = (table)[m];
        m++;
        i++;
    }
    
    s = start;
    
    for (int j = 0; j < i; j++) {
        
        (table)[s] = temp[j];
        s++;
    }
}
