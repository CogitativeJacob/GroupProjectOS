/*
* Group G
* Hagen Patterson
* hagen.patterson@okstate.edu
* 4/7/2024
*/

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
    pthread_t thread0;
    pthread_mutex_t lock0;
    pthread_t thread1;
    pthread_mutex_t lock1;
    pthread_t thread2;
    pthread_mutex_t lock2;
    pthread_t thread3;
    pthread_mutex_t lock3;
    pthread_t thread4;
    pthread_mutex_t lock4;
    pthread_t thread5;
    pthread_mutex_t lock5;
    pthread_mutex_t MQlock; // For mutual exclusion
    pthread_cond_t cond; // For signaling
} MonitorQueue;

void enterMonitor(MonitorQueue* q, Transaction* t); // has transaction argument for testing
void exitMonitor(MonitorQueue* q);

void* grabWorker(void* transaction);

#endif
