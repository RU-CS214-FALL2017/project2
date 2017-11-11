#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "forkTools.h"
#include "tools.h"
#include "memTools.h"
#include "sorter.h"

// Sorts all CSV files by the column header <columnHeader> in the
// directory and subdirectories located at <path>. The sorted CSV
// file is stored in the directory of the path <outputDir>. <info>'s
// refrence will be set to point to newly mapped memory of info
// about the processed directory. To free, unmap all members of
// *(*<info>) and all members of subDirs within *(*<info>).
void processCsvDir(const char * path, struct sharedMem * sharedMem,
                  const char * columnHeaders, const char * outputDir) {
    
    DIR * dir = opendir(path);
    
    struct csvDir * info = getDirSeg(sharedMem, getpid());
    info->path = (char *) myalloc(strlen(path) + 1, sharedMem);
    strcpy(info->path, path);
    
    pid_t * tempDirPids = (pid_t *) myMap(sizeof(pid_t) * TEMPSIZE);
    info->numSubDirs = 0;
    
    pid_t * tempCsvPids = (pid_t *) myMap(sizeof(pid_t) * TEMPSIZE);
    info->numCsvs = 0;
    
    for (struct dirent * entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {

            pid_t child = fork();
            
            if (child == 0) {
                
                char subDirPath[TEMPSIZE];
                sprintf(subDirPath, "%s/%s", path, entry->d_name);
                processCsvDir(subDirPath, sharedMem, columnHeaders, outputDir);
                
                exit(EXIT_SUCCESS);
            }

            tempDirPids[info->numSubDirs] = child;
            (info->numSubDirs)++;
            printf("%d,", child);
            fflush(stdout);
            
        } else if (entry->d_type == DT_REG) {
            
            char csvPath[TEMPSIZE];
            sprintf(csvPath, "%s/%s", path, entry->d_name);
            
            if (isCsv(csvPath)) {
                
                pid_t child = fork();
                
                if (child == 0) {
                    
                    if (outputDir == NULL) {
                        sortCsv(csvPath, columnHeaders, path, sharedMem);
                        
                    } else {
                        sortCsv(csvPath, columnHeaders, outputDir, sharedMem);
                    }
                    
                    exit(EXIT_SUCCESS);
                }
                
                tempCsvPids[info->numCsvs] = child;
                
                (info->numCsvs)++;
                printf("%d,", child);
                fflush(stdout);
            }
        }
    }
    
    closedir(dir);
    
    info->subDirsPids = (pid_t *) myalloc(sizeof(pid_t) * info->numSubDirs, sharedMem);
    memcpy(info->subDirsPids, tempDirPids, sizeof(pid_t) * info->numSubDirs);
    munmap(tempDirPids, sizeof(pid_t) * TEMPSIZE);
    
    info->csvPids = (pid_t *) myalloc(sizeof(pid_t) * info->numCsvs, sharedMem);
    memcpy(info->csvPids, tempCsvPids, sizeof(pid_t) * info->numCsvs);
    munmap(tempCsvPids, sizeof(pid_t) * TEMPSIZE);
    
    for (int i = 0; i < info->numSubDirs + info->numCsvs; i++) {
        wait(NULL);
    }
}
