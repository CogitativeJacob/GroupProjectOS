#include "user_queue.h"
#include <string.h>
#include <stdlib.h>
#include "transactions.h"
#include <stdio.h>

// user_queue.c
UserQueue userQueues[MAX_USERS];
int userCount = 0;


// Finds or creates a queue for the specified account number
UserQueue* findOrCreateUserQueue(const char* accountNumber) {
    // For simplicity, this example searches for an existing queue or creates a new one at the end
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userQueues[i].front->accountNumber, accountNumber) == 0) {
            return &userQueues[i];
        }
    }
    // If not found, initialize a new queue
    if (userCount < MAX_USERS) {
        UserQueue* newQueue = &userQueues[userCount++];
        newQueue->front = NULL;
        newQueue->rear = NULL;
        return newQueue;
    }
    return NULL; // In a real application, handle this case properly
}

void enqueueTransaction(const char* accountNumber, Transaction* transaction) {
    UserQueue* queue = findOrCreateUserQueue(accountNumber);
    if (!queue) return; // Handle error or full array

    transaction->next = NULL;
    if (queue->rear == NULL) {
        // Empty queue
        queue->front = queue->rear = transaction;
    } else {
        // Add to the end and update rear
        queue->rear->next = transaction;
        queue->rear = transaction;
    }
}

// Assuming this is in user_queue.c or a similar source file

Transaction* dequeueTransaction(const char* accountNumber) {
    // Your logic to find the user's queue goes here
    // For simplicity, let's assume you find the queue and it's not empty

    UserQueue* queue = findOrCreateUserQueue(accountNumber);
    printf("Dequeuing transaction for account: %s\n", accountNumber);


    if (queue != NULL && queue->front != NULL) {
        Transaction* transaction = queue->front;
        queue->front = queue->front->next;
        if (queue->front == NULL) {
            queue->rear = NULL; // If the queue is now empty, update the rear pointer
        }
        printf("Dequeued transaction: %s, %.2f\n", transaction->accountNumber, transaction->amount);
        return transaction;
    }
    return NULL; // If the queue is empty or not found
}

