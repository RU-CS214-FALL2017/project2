#include <pthread.h>
#include <stdlib.h>

#include "queue.h"

pthread_cond_t CV = PTHREAD_COND_INITIALIZER;
pthread_mutex_t QM = PTHREAD_MUTEX_INITIALIZER;

struct node * head;
struct node * tail;

unsigned int QElements = 0;

pthread_t popTid() {
    
    pthread_mutex_lock(&QM);
    
    QElements--;
    
    struct node * oldHead = head;
    head = oldHead->next;
    
    pthread_mutex_unlock(&QM);
    
    pthread_t ret = oldHead->tid;
    free(oldHead);
    
    return ret;
}

void pushTid(pthread_t tid) {
    
    struct node * newNode = (struct node *) malloc(sizeof(struct node));
    newNode->tid = tid;
    
    pthread_mutex_lock(&QM);
    
    if (QElements == 0) {
        head = newNode;
        
    } else {
        tail->next = newNode;
    }
    
    tail = newNode;
    
    QElements++;
    
    if(QElements > 1) {
        pthread_cond_signal(&CV);
    }
    
    pthread_mutex_unlock(&QM);
}
