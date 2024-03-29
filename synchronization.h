#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "transactions.h"
#include <pthread.h>
#include <unistd.h> 

typedef struct QueueNode {
    pid_t pid; // id of thread
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
    pthread_mutex_t lock; // For mutual exclusion
    pthread_cond_t cond; // For signaling
} MonitorQueue;

void enterMonitor(MonitorQueue* q, Transaction* t); // has transaction argument for testing
void exitMonitor(MonitorQueue* q);

#endif
