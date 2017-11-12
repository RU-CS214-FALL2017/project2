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

// <row> is the address to a char **. Creates a array of strings
// A, where each comma seperated value from <line> is an element
// of A, and <row>'s refrence is set to point to A. Returns the
// number elements in A (columns). To free, free each (*<row>)[i]
// (0 <= i < # of elements in A) and free *<row>.
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

// Removes leading and trailing whitespaces from <str>.
void trim (char * str) {
    
    int i = 0;
    
    while(str[i] == ' ' || str[i] == '\n' || str[i] == '\t' || str[i] == '\r' || str[i] == '\v'
          || str[i] == '\f') {
        i += 1;
    }
    
    removeChars(str, 0, i);
    
    unsigned long terminatingIndex = strlen(str);
    unsigned long j = terminatingIndex - 1;
    
    while(str[j] == ' ' || str[j] == '\n' || str[j] == '\t' || str[j] == '\r' || str[j] == '\v'
          || str[j] == '\f') {
        j -= 1;
    }
    
    removeChars(str, j + 1, terminatingIndex);
}

// Removes the characters from <str> between indexes <startIndex> (inclusive)
// and <endIndex> (exclusive).
void removeChars (char * str, unsigned long startIndex, unsigned long endIndex) {
    
    unsigned long terminatingIndex = strlen(str);
    
    for(int i = 0; i <= (terminatingIndex - endIndex); i++) {
        
        str[startIndex + i] = str[endIndex + i];
    }
}

// <table> is an address to a char ***. <rows> is an address to
// an unsigned int. <columns> is an address to an unsigned int.
// Creates a "table" from <csvFile> as a 2D string array, where
// A[n][m] will return a string representation of the data stored
// in the (n+1)th row and (m+1)th column of the "table". <table>'s
// refrence is set to point to the created "table". <rows>' refrence
// is set to the numbers of rows in "table". <columns>' refrence is
// set to the number of columns in "table". To free, free each
// (*<table>)[i][j] (0 <= i < *<rows>, 0 <= j < *<columns>) and free
// *<table>.
//void fillTable(FILE * csvFile, char * *** table, unsigned int * rows, unsigned int * columns) {
//    
//    *table = (char ***) malloc(TEMPSIZE * TEMPSIZE * sizeof(char **));
//    char line[TEMPSIZE];
//    *rows = 0;
//    *columns = 0;
//    
//    while(fgets(line, TEMPSIZE, csvFile) != NULL) {
//        
//        int tempColumns = tokenizeRow(line, &(*table)[*rows]);
//        
//        if (*rows == 0) {
//            
//            *columns = tempColumns;
//            (*rows)++;
//            
//        } else if (tempColumns == *columns) {
//            (*rows)++;
//            
//        } else {
//            doubleFree((*table)[*rows], tempColumns);
//        }
//    }
//    
//    *table = (char ***) realloc(*table, sizeof(char **) * *rows);
//}

// Prints <table> with <rows> rows and <columns> columns in a
// csv (comma seperated values) format to <stream>.
void printTable (FILE * stream, char *** table, unsigned int rows, unsigned int columns) {
    
    for (int i = 0; i < rows; i++) {
        
        if(strchr(table[i][0], ',') != NULL) {
            fprintf(stream, "\"%s\"", table[i][0]);
            
        } else {
            fprintf(stream, "%s", table[i][0]);
        }
        
        for (int j = 1; j < columns; j++) {
            
            if(strchr(table[i][j], ',') != NULL) {
                fprintf(stream, ",\"%s\"", table[i][j]);
                
            } else {
                fprintf(stream, ",%s", table[i][j]);
            }
        }
        
        fprintf(stream, "\n");
    }
}

// Returns 1 if <str> is a valid number, else returns 0.
int isNumber(const char * str) {
    
    for (int i = 0; i < strlen(str); i++) {
        
        if (!(isdigit(str[i]) || (str[i] == '.'))) {
            return 0;
        }
    }

    return 1;
}

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

// If all the values in <table> at column index <columnIndex> from row index 1
// to row index <rows> - 1 are valid numbers, returns 1, else returns 0.
int isNumericColumn(char *** table, int rows, int columnIndex) {
    
    for (int i = 1; i < rows; i++) {
        
        if (!isNumber(table[i][columnIndex])) {
            return 0;
        }
    }
    
    return 1;
}

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

