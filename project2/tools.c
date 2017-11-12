#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "tools.h"
#include "forkTools.h"
#include "memTools.h"

int findCsvFilesHelper(const char * dirPath, char ** csvPaths, int * numFound);
void printDirTreeHelper(FILE * output, pid_t pid, struct sharedMem * sharedMem, unsigned int level);

// Tokenizes a CSV row into the pre-allocated <row>. <line> is the
// pre-allocated CSV line/row to tokenize. Returns 1 if 27 cells
// have been tokenized, otherwise returns 0.
int tokenizeRow(char * line, char ** row) {
    
    int rc = 0;
    char * tempCell = line;
    char * endLine = line + strlen(line);
    
    int inQuote = 0;
    int beginCell = 1;
    
    while(line < endLine) {
        
        if(*line == ',' && !inQuote && !beginCell) {
                
            *line = '\0';
            row[rc] = tempCell;
            rc++;
            beginCell = 1;
            
        } else if (*line == '"') {
            
            if (inQuote) {
                
                *line = '\0';
                row[rc] = tempCell;
                rc++;
                beginCell = 1;
                
            } else {
                
                inQuote = 1;
                beginCell = 1;
            }
            
        } else if (beginCell && *line != ' ') {
            
            if (*line == ',') {
                
                if (inQuote) {
                    inQuote = 0;
                    
                } else {
                    
                    *line = '\0';
                    row[rc] = line;
                    rc++;
                }
                
            } else {
                
                tempCell = line;
                beginCell = 0;
            }
        }
        
        line++;
    }
    
    row[rc] = tempCell;
    
    if (rc == 27) {
        return 1;
    }
    
    return 0;
    
//    row->color = charRow[0];
//    row->director_name = charRow[1];
//    row->num_critic_for_reviews = atof(charRow[2]);
//    row->duration = atof(charRow[3]);
//    row->director_facebook_likes = atof(charRow[4]);
//    row->actor_3_facebook_likes = atof(charRow[5]);
//    row->actor_2_name = charRow[6];
//    row->actor_1_facebook_likes = atof(charRow[7]);
//    row->gross = atof(charRow[8]);
//    row->genres = charRow[9];
//    row->actor_1_name = charRow[10];
//    row->movie_title = charRow[11];
//    row->num_voted_users = atof(charRow[12]);
//    row->cast_total_facebook_likes = atof(charRow[13]);
//    row->actor_3_name = charRow[14];
//    row->facenumber_in_poster = atof(charRow[15]);
//    row->plot_keywords = charRow[16];
//    row->movie_imdb_link = charRow[17];
//    row->num_user_for_reviews = atof(charRow[18]);
//    row->language = charRow[19];
//    row->country = charRow[20];
//    row->content_rating = charRow[21];
//    row->budget = atof(charRow[22]);
//    row->title_year = atof(charRow[23]);
//    row->actor_2_facebook_likes = atof(charRow[24]);
//    row->imdb_score = atof(charRow[25]);
//    row->aspect_ratio = atof(charRow[26]);
//    row->movie_facebook_likes = atof(charRow[27]);
}

//// Removes leading and trailing whitespaces from <str>.
//void trim (char * str) {
//    
//    int i = 0;
//    
//    while(str[i] == ' ' || str[i] == '\n' || str[i] == '\t' || str[i] == '\r' || str[i] == '\v'
//          || str[i] == '\f') {
//        i += 1;
//    }
//    
//    removeChars(str, 0, i);
//    
//    unsigned long terminatingIndex = strlen(str);
//    unsigned long j = terminatingIndex - 1;
//    
//    while(str[j] == ' ' || str[j] == '\n' || str[j] == '\t' || str[j] == '\r' || str[j] == '\v'
//          || str[j] == '\f') {
//        j -= 1;
//    }
//    
//    removeChars(str, j + 1, terminatingIndex);
//}

//// Removes the characters from <str> between indexes <startIndex> (inclusive)
//// and <endIndex> (exclusive).
//void removeChars (char * str, unsigned long startIndex, unsigned long endIndex) {
//
//    unsigned long terminatingIndex = strlen(str);
//
//    for(int i = 0; i <= (terminatingIndex - endIndex); i++) {
//
//        str[startIndex + i] = str[endIndex + i];
//    }
//}

// Fills pre-allocated <table> with data from <csvFile>. <cells> is a
// pre-allocated memory that will be stored with strings of the cells.
// Returns the number of rows in table. To free, free <table>[i], 0 <
// i < return value.
unsigned int fillTable(FILE * csvFile, char *** table, char * cells) {
    
    unsigned int rows = 0;
    char * i = cells;
    
    int dontAlloc = 0;
    
    while(fgets(i, TEMPSIZE, csvFile) != NULL) {
        
        if (dontAlloc) {
            dontAlloc = 0;
            
        } else {
            table[rows] = (char **) malloc(sizeof(char *) * COLUMNS);
        }
        
        unsigned long next = strlen(i) + 1;
        
        int goodRow = tokenizeRow(i, table[rows]);
        
        if (goodRow) {
            
            rows++;
            i += next;
            
        } else {
            dontAlloc = 1;
        }
        
    }
    
    if(dontAlloc == 1) {
        free(table[rows]);
    }
    
    return rows;
}

// Prints <table> with <rows> rowscolumns in a csv
// (comma seperated values) format to <stream>.
void printTable (FILE * stream, char *** table, unsigned int rows) {
    
    for (int i = 0; i < rows; i++) {
        
        if(strchr(table[i][0], ',') != NULL) {
            fprintf(stream, "\"%s\"", table[i][0]);
            
        } else {
            fprintf(stream, "%s", table[i][0]);
        }
        
        for (int j = 1; j < COLUMNS; j++) {
            
            if(strchr(table[i][j], ',') != NULL) {
                fprintf(stream, ",\"%s\"", table[i][j]);
                
            } else {
                fprintf(stream, ",%s", table[i][j]);
            }
        }
    }
}

//// Returns 1 if <str> is a valid number, else returns 0.
//int isNumber(const char * str) {
//
//    for (int i = 0; i < strlen(str); i++) {
//
//        if (!(isdigit(str[i]) || (str[i] == '.'))) {
//            return 0;
//        }
//    }
//
//    return 1;
//}

// If <areNumbers> is set to 0, returns 0 if <y> is lexicographically before <x>,
// else returns 1. If <areNumbers> is set to anything besides 0, converts <x> and
// <y> to double values x y respectively, and returns x <= y.
int isXBeforeY (const char * x, const char * y, int areNumbers) {
    
    if (areNumbers) {
        return atof(x) <= atof(y);
        
    } else {
        return (strcmp(x, y) <= 0);
    }
}

//// If all the values in <table> at column index <columnIndex> from row index 1
//// to row index <rows> - 1 are valid numbers, returns 1, else returns 0.
//int isNumericColumn(char *** table, int rows, int columnIndex) {
//
//    for (int i = 1; i < rows; i++) {
//
//        if (!isNumber(table[i][columnIndex])) {
//            return 0;
//        }
//    }
//
//    return 1;
//}

// Returns 1 if <csvPath> is a path to a "proper" .csv file, else returns 0.
int isCsv(const char * csvPath) {
    
    const char * extension = csvPath + strlen(csvPath) - 4;
    
    if (!strcmp(".csv", extension)) {
        return 1;
    }
    
    return 0;
}

// If the name of the CSV file located at <path> is A. This function returns
// the newly allocated string: "<outputDir>/A-sorted-<columnHeader>.csv". To
// free, free the returned pointer.
char * sortedCsvPath(const char * csvPath, const char * columnHeader, const char * outputDir) {
    
    char temp[strlen(csvPath) + 1];
    strcpy(temp, csvPath);
    
    char * token = strtok(temp, "/");
    char * tempTok = NULL;
    
    while (token != NULL) {
        
        tempTok = token;
        token = strtok(NULL, "/");
    }
    
    tempTok[strlen(tempTok) - 4] = '\0';
    
    char * ret = (char * ) malloc(strlen(outputDir) + strlen(tempTok) + strlen(columnHeader) + 14);
    sprintf(ret, "%s/%s-sorted-%s.csv", outputDir, tempTok, columnHeader);
    
    return ret;
}

// Returns the index of <columnHeader> in <table> with <columns>
// columns. If <columnHeader> not found, return -1.
int getColumnHeaderIndex(const char * columnHeader,
                         char *** table, const unsigned int columns) {
    
    for (int i = 0; i < columns; i++) {
        
        if (!strcmp(columnHeader, table[0][i])) {
            return i;
        }
    }
    
    return -1;
}

void printDirTree(FILE * output, struct sharedMem * sharedMem) {
    printDirTreeHelper(output, getpid(), sharedMem, 0);
}

void printDirTreeHelper(FILE * output, pid_t pid, struct sharedMem * sharedMem, unsigned int level) {
    
    struct csvDir * dir = getDirSeg(sharedMem, pid);
    
    char * begin = "| ";
    char * end = "|-";
    
    for (int i = 0; i < level; i++){
        fprintf(output, "%s", begin);
    }
    
    fprintf(output, "%s%d: Processed the directory %s\n", end, pid, dir->path);
    
    for (int i = 0; i < dir->numCsvs; i++) {
        
        for (int i = 0; i < (level + 1); i++){
            fprintf(output, "%s", begin);
        }
        
        struct csv * csv = getCsvSeg(sharedMem, dir->csvPids[i]);
        
        if(csv->sorted) {
            
            fprintf(output, "%s%d: Sorted the file %s", end, (dir->csvPids)[i], csv->path);
            
            if (csv->error) {
                fprintf(output, " (%s)", csv->errors);
            }
            
            fprintf(output, "\n");
            
        } else {
            
            fprintf(output, "%s%d: Tried to sort the file %s (%s)\n", end, (dir->csvPids)[i], csv->path, csv->errors);
        }
    }
    
    for (int i = 0; i < dir->numSubDirs; i++) {
        printDirTreeHelper(output, (dir->subDirsPids)[i], sharedMem, level + 1);
    }
}

unsigned int dirSubProcessCount(pid_t dirPid, struct sharedMem * sharedMem) {
    
    struct csvDir * dir = getDirSeg(sharedMem, dirPid);
    
    unsigned int count = 1 + dir->numCsvs;
    
    for (int i = 0; i < dir->numSubDirs; i++) {
        count += dirSubProcessCount((dir->subDirsPids)[i], sharedMem);
    }
    
    return count;
}

// Checks weather <path> is a valid directory, if not,
// the program crashes with an approiate error message.
void checkDir(const char * path, const char * dirType) {

    DIR * dir = opendir(path);
    
    if (dir == NULL) {
        
        errno = 0;
        
        switch (errno) {
                
            case EACCES:
                printf("You do not have access to the specified %s directory, %s\n", dirType, path);
                break;
                
            case ENOENT:
                printf("The specified %s directory, %s, does not exist\n", dirType, path);
                break;
                
            case ENOTDIR:
                printf("The specified %s directory, %s, is not a directory\n", dirType, path);
                break;
                
            default:
                printf("A problem occured opening the specified %s directory, %s", dirType, path);
                break;
        }
        
        closedir(dir);
        exit(EXIT_FAILURE);
    }
    
    closedir(dir);
}

