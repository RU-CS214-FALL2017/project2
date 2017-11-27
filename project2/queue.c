#include <pthread.h>
#include <stdlib.h>

#include "queue.h"

pthread_cond_t CV = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

struct node * head;
struct node * tail;

unsigned int elements = 0;

struct table * popTable() {
    
    pthread_mutex_lock(&m);
    
    elements--;
    
    struct node * oldHead = head;
    head = oldHead->next;
    
    pthread_mutex_unlock(&m);
    
    struct table * ret = oldHead->table;
    free(oldHead);
    
    return ret;
}

void pushTable(struct table * table) {
    
    struct node * newNode = (struct node *) malloc(sizeof(struct node));
    newNode->table = table;
    
    pthread_mutex_lock(&m);
    
    tail->next = newNode;
    tail = newNode;
    
    elements++;
    
    if(elements > 1) {
        pthread_cond_signal(&CV);
    }
    
    pthread_mutex_unlock(&m);
}
