#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

//#include "tools.h"
#include "sorter.h"
#include "memTools.h"

pthread_mutex_t m;
struct table * * tables;
unsigned int tc = 0;

void addTable(struct table * table) {
    
    pthread_mutex_lock(&m);
    
    tables[tc] = table;
    tc++;
    
    pthread_mutex_unlock(&m);
}

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
    
    if (!isCsv(params->path)) {
        
        fprintf(stderr, "Not a CSV file: %s\n", params->path);
        fflush(stderr);
        free(threadParams);
        pthread_exit(NULL);
    }
    
    struct table * table = malloc(sizeof(struct table));
    
    if (!fillTable(params->path, table)) {
        
        fprintf(stderr, "Not a proper movie_metadata CSV file: %s\n", params->path);
        fflush(stderr);
        free(threadParams);
        pthread_exit(NULL);
    }
    
    mergeSort(table->table, params->sortIndex, params->isNumeric, 1, table->numRows);
    printToSortedCsvPath(params->path, params->header, params->output, table->table, table->numRows);
    
    addTable(table);
    
//    freeTable(table);
//
//    printf("sorted, %s\n", params->path);
//    fflush(stdout);
    
    free(threadParams);
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
        
        if(isXBeforeY(table[s][columnIndex], table[m][columnIndex], isNumeric)) {
            
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
        
        table[s] = temp[j];
        s++;
    }
}

void * mergeTables(void * parameters) {

    struct mergeTablesParams * params = (struct mergeTablesParams *) parameters;
    
    if (params->numTables == 1) {
        pthread_exit(params->tables);
        
    } else if (params->numTables == 2) {
        
        struct table * ftable = *(params->tables);
        struct table * stable = *(params->tables + 1);
        struct table * table = malloc(sizeof(struct table));
        table->table = malloc(sizeof(char **) * (ftable->numRows + stable->numRows));
        
        free(params->tables);
        
        unsigned int fc = 0;
        unsigned int sc = 0;
        unsigned int tc = 0;
        
        while (fc < ftable->numRows && sc < stable->numRows) {
            
            if (isXBeforeY(ftable->table[fc][params->sortIndex], stable->table[sc][params->sortIndex], params->isNumeric)) {
                
                table->table[tc] = ftable->table[fc];
                fc++;
                
            } else {
                
                table->table[tc] = stable->table[sc];
                sc++;
            }
            
            tc++;
        }
        
        while (fc < ftable->numRows) {
            
            table->table[tc] = ftable->table[fc];
            fc++;
            tc++;
        }
        
        while (sc < stable->numRows) {
            
            table->table[tc] = stable->table[sc];
            sc++;
            tc++;
        }
        
        free(ftable->table);
        free(stable->table);
        
        table->rowsMems = malloc(sizeof(char *) * (ftable->numRowsMems + stable->numRowsMems));
        unsigned int trmc = 0;
        for (int i = 0; i < ftable->numRowsMems; i++) {
            table->rowsMems[trmc] = ftable->rowsMems[i];
            trmc++;
        }
        for (int i = 0; i < stable->numRowsMems; i++) {
            table->rowsMems[trmc] = stable->rowsMems[i];
            trmc++;
        }
        table->numRowsMems = trmc;
        free(ftable->rowsMems);
        free(stable->rowsMems);
        
        table->cellsMems = malloc(sizeof(char *) * (ftable->numCellsMems + stable->numCellsMems));
        unsigned int tcmc = 0;
        for (int i = 0; i < ftable->numCellsMems; i++) {
            table->cellsMems[tcmc] = ftable->cellsMems[i];
            tcmc++;
        }
        for (int i = 0; i < stable->numRows; i++) {
            table->cellsMems[tcmc] = stable->cellsMems[i];
            tcmc++;
        }
        table->numCellsMems = tcmc;
        free(ftable->cellsMems);
        free(stable->cellsMems);
        
        pthread_exit(table);
        
    } else {
        
        unsigned int mid = params->numTables / 2;
        
        struct mergeTablesParams fparam;
        fparam.isNumeric = params->isNumeric;
        fparam.numTables = mid;
        fparam.sortIndex = params->sortIndex;
        fparam.tables = params->tables;
        
        struct mergeTablesParams sparam;
        sparam.isNumeric = params->isNumeric;
        sparam.numTables = params->numTables - mid;
        sparam.sortIndex = params->sortIndex;
        sparam.tables = params->tables + mid;
        
        pthread_t threads[2];
        pthread_create(&threads[0], NULL, mergeTables, &fparam);
        pthread_create(&threads[1], NULL, mergeTables, &sparam);
        void * fret;
        void * sret;
        pthread_join(threads[0], &fret);
        pthread_join(threads[1], &sret);
        
        struct table * ftable = fret;
        struct table * stable = sret;
        
        struct mergeTablesParams mergeParams;
        mergeParams.isNumeric = params->isNumeric;
        mergeParams.numTables = 2;
        mergeParams.tables = malloc(sizeof(struct table *) * 2);
        mergeParams.tables[0] = ftable;
        mergeParams.tables[1] = stable;
        
        mergeTables(&mergeParams);
        return 0;
    }
}


