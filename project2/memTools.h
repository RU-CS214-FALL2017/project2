#ifndef memTools_h
#define memTools_h

//#define META_SIZE sizeof(struct metadata)
//#define DIR_MEM_SIZE (TEMPSIZE * sizeof(struct csvDir))
//#define CSV_MEM_SIZE (TEMPSIZE * sizeof(struct csv))
//#define SHARED_MEM_SIZE ((TEMPSIZE * TEMPSIZE) + sizeof(struct sharedMem))

//struct sharedMem {
//  
//    pid_t pid;
//    void * dirMem;
//    void * csvMem;
//};

// Represents metadata for my memory manager.
//struct metadata {
//
//    int64_t size;
//    int64_t dirty;
//};

#include "tools.h"

void freeTable(struct table table);
void reAllocTable(struct table * table);
//void doubleFree(char ** alloc, int x);
//void tripleFree(char *** alloc, int x, int y);
//void * myMap(size_t size);
//struct sharedMem * initSharedMem(void);
//void freeSharedMem(struct sharedMem * sharedMem);
//struct csvDir * getDirSeg(struct sharedMem * sharedMem, pid_t pid);
//struct csv * getCsvSeg(struct sharedMem * sharedMem, pid_t pid);
//void * myalloc(size_t size, struct sharedMem * sharedMem);

#endif /* memTools_h */
