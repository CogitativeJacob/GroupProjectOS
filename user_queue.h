#ifndef USER_QUEUE_H
#define USER_QUEUE_H

#include "transactions.h"
#include <pthread.h>

#define MAX_USERS 100

typedef struct QueueNode {
    Transaction transaction;
    struct QueueNode* next;
} QueueNode;

typedef struct TransactionQueue {
    char accountNumber[20];  // To identify the queue by account number
    QueueNode* front;        // Pointer to the front of the queue
    QueueNode* rear;         // Pointer to the rear of the queue
    int isActive;
} TransactionQueue;

extern TransactionQueue userQueues[MAX_USERS];  // Array of transaction queues
extern int userCount;                           // Number of active queues

// Queue operations
void enqueueTransaction(const char* accountNumber, Transaction transaction);
Transaction* dequeueTransaction(const char* accountNumber);
void printUserQueues();

#endif
