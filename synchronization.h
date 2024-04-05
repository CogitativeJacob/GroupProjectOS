#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "transactions.h"
#include <pthread.h>
#include <unistd.h> 

typedef struct jobNode {
    Transaction* job;
    struct jobNode* next;
} jobNode;

typedef struct workerQueue {
    jobNode* front;
    jobNode* rear;
    transType transactionType;
    pthread_mutex_t jlock;
    pthread_cond_t jcond;
} workerQueue;

typedef struct QueueNode {
    pid_t pid; // id of thread
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
    workerQueue* createQueue;
    workerQueue* depositQueue;
    workerQueue* withdrawQueue;
    workerQueue* inquiryQueue;
    workerQueue* transferQueue;
    workerQueue* closeQueue;
    pthread_mutex_t lock; // For mutual exclusion
    pthread_cond_t cond; // For signaling
} MonitorQueue;

void enterMonitor(MonitorQueue* q, Transaction* t); // has transaction argument for testing
void exitMonitor(MonitorQueue* q);

void addjob(workerQueue* q, Transaction* t);
void popjob(workerQueue* q, int* noMoreJobs);


#endif
