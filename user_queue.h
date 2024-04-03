#ifndef USER_QUEUE_H
#define USER_QUEUE_H

#include "transactions.h"
#define MAX_USERS 100 // For demonstration, a fixed size

typedef struct UserQueue {
    Transaction* front; // Front of the queue
    Transaction* rear; // Rear of the queue
} UserQueue;

// user_queue.h
extern UserQueue userQueues[MAX_USERS];
extern int userCount;

void printUserQueues();
void enqueueTransaction(const char* accountNumber, Transaction* transaction);
Transaction* dequeueTransaction(const char* accountNumber);

#endif
