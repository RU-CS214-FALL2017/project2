#ifndef tools_h
#define tools_h

#include <stdio.h>
#include <sys/types.h>

#include "forkTools.h"

struct row {

    char color;
    char director_name;
    unsigned int num_critic_for_reviews;
    unsigned int duration;
    unsigned int director_facebook_likes;
    unsigned int actor_3_facebook_likes;
    char actor_2_name;
    unsigned int actor_1_facebook_likes;
    unsigned long gross;
    char genres;
    char actor_1_name;
    char movie_title;
    unsigned long num_voted_users;
    unsigned int cast_total_facebook_likes;
    char actor_3_name;
    int facenumber_in_poster;
    char plot_keywords;
    char movie_imdb_link;
    unsigned int num_user_for_reviews;
    char language;
    char country;
    char content_rating;
    double budget;
    unsigned int title_year;
    unsigned int actor_2_facebook_likes;
    double imdb_score;
    double aspect_ratio;
    unsigned int movie_facebook_likes;
};

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
