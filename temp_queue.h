#ifndef TEMP_QUEUE_H
#define TEMP_QUEUE_H

#include "transactions.h"
#include <pthread.h>
#include <unistd.h> 

typedef struct{
    struct Transaction* head;
    struct Transaction* tail;
    int length;
    pthread_mutex_t lock;
} userQueue;

int enQueueJob(userQueue* q, Transaction* t);
Transaction* dequeue(userQueue* q);

#endif