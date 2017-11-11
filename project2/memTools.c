#define _GNU_SOURCE 1
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>

#include "forkTools.h"
#include "memTools.h"

#define META_PTR_CAST (struct metadata *)
#define CHAR_PTR_CAST (char *)
#define VOID_PTR_CAST (void *)
#define PID_PTR_CAST (pid_t *)
#define DIR_PTR_CAST (struct csvDir *)
#define SHARED_MEM_PTR_CAST (struct sharedMem *)

void * initMemHelper(size_t size);
void * getMySharedSeg(struct sharedMem * sharedMem);
void initMetadata(void * memSeg, size_t size);

// Frees <alloc>[i] (0 <= i < <x>) and <alloc>.
void doubleFree(char ** alloc, int x) {
    
    for (int i = 0; i < x; i++) {
        free(alloc[i]);
    }
    
    free(alloc);
}

// Frees <alloc>[i][j] (0 <= i < <x>, 0 <= j < <y>) and <alloc>.
void tripleFree(char *** alloc, int x, int y) {
    
    for (int i = 0; i < x; i++) {
        
        for (int j = 0; j < y; j++) {
            free(alloc[i][j]);
        }
        
        free(alloc[i]);
    }
    
    free(alloc);
}

// Allocates <size> bytes of memory shared between processes
// and returns a pointer to the allocated memory. To free,
// unmap the returned pionter with <size>.
void * myMap(size_t size) {
    return mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

// Initializes and returns shared memory that can be used
// with myalloc. To free, unmap the returned pointer with
// size SHARED_MEM_SIZE.
struct sharedMem * initSharedMem() {
    
    struct sharedMem * ret = myMap(SHARED_MEM_SIZE);
    
    ret->pid = getpid();
    ret->csvMem = myMap(CSV_MEM_SIZE);
    ret->dirMem = myMap(DIR_MEM_SIZE);
    
    char * seg = (CHAR_PTR_CAST ret) + sizeof(struct sharedMem);
    
    while (seg < (CHAR_PTR_CAST ret) + (SHARED_MEM_SIZE - sizeof(struct sharedMem))) {
        
        initMetadata(VOID_PTR_CAST seg, TEMPSIZE);
        seg += TEMPSIZE;
    }
    
    return ret;
}

void freeSharedMem(struct sharedMem * sharedMem) {
    
    munmap(sharedMem->dirMem, DIR_MEM_SIZE);
    munmap(sharedMem->csvMem, CSV_MEM_SIZE);
    munmap(sharedMem, SHARED_MEM_SIZE);
}

// Retrieves the info of the directory process <pid> from <dirMem>.
struct csvDir * getDirSeg(struct sharedMem * sharedMem, pid_t pid) {
    
    pid_t index = pid - sharedMem->pid;
    return ((struct csvDir *) sharedMem->dirMem) + index;
}

// Retrieves the info of the csv process <pid> from <csvMem>.
struct csv * getCsvSeg(struct sharedMem * sharedMem, pid_t pid) {
    
    pid_t index = pid - sharedMem->pid;
    return ((struct csv *) sharedMem->csvMem) + index;
}

// Retrieves the segregated memory of the current pid
// in <sharedMem>.
void * getMySharedSeg(struct sharedMem * sharedMem) {
    
    pid_t index = getpid() - sharedMem->pid;
    char * ret = CHAR_PTR_CAST (sharedMem + 1);
    return VOID_PTR_CAST (ret + (TEMPSIZE * index));
}

// Adds clean metadata to <memSeg> of <size>.
void initMetadata(void * memSeg, size_t size) {

    struct metadata * header = META_PTR_CAST memSeg;

    header->size = size;
    header->dirty = 0;
}

// Returns allocated memory of <size> from sharedMem of the current pid.
void * myalloc(size_t size, struct sharedMem * sharedMem) {

    void * mem = getMySharedSeg(sharedMem);
    char * alloc = CHAR_PTR_CAST mem;

    while(alloc < (CHAR_PTR_CAST mem) + TEMPSIZE) {

        if (!(META_PTR_CAST alloc)->dirty && (META_PTR_CAST alloc)->size >= size) {

            int64_t oldSize = (META_PTR_CAST alloc)->size;
            
            (META_PTR_CAST alloc)->dirty = 1;
            (META_PTR_CAST alloc)->size = size;
            char * nextHeader = alloc + size + META_SIZE;
            initMetadata(VOID_PTR_CAST nextHeader, oldSize - size - META_SIZE);

            return (VOID_PTR_CAST alloc) + META_SIZE;
        }

        alloc += ((META_PTR_CAST alloc)->size + META_SIZE);
    }
    
    return NULL;
}

