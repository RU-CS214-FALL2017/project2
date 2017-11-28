#ifndef queue_h
#define queue_h

#include "tools.h"

extern pthread_cond_t CV;
extern unsigned int QElements;

struct node {
  
    pthread_t tid;
    struct node * next;
};

pthread_t popTid(void);
void pushTid(pthread_t tid);

#endif /* queue_h */
