/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/21/2024
*/

#ifndef USER_QUEUE_H
#define USER_QUEUE_H

#include "transactions.h"
#include <pthread.h>

#define MAX_USERS 100  // Defines the maximum number of user transaction queues

/**
 * Represents a node in a transaction queue.
 * Each node contains a transaction and a pointer to the next node in the queue.
 */
typedef struct QueueNode {
    Transaction transaction;  // The transaction data stored in this node
    struct QueueNode* next;   // Pointer to the next node in the queue
} QueueNode;

/**
 * Represents a queue of transactions for a single user account.
 */
typedef struct TransactionQueue {
    char accountNumber[20];   // Identifier for the account associated with this queue
    QueueNode* front;         // Pointer to the front of the queue (where transactions are dequeued)
    QueueNode* rear;          // Pointer to the rear of the queue (where transactions are enqueued)
    int isActive;             // Flag to indicate if the queue is active
} TransactionQueue;

// Global array storing the transaction queues for all users
extern TransactionQueue userQueues[MAX_USERS];
// Global counter for the number of active transaction queues
extern int userCount;

/**
 * Enqueues a transaction to the transaction queue of the specified account.
 * If the account's queue does not exist, it creates a new queue.
 * 
 * @param accountNumber The account number associated with the transaction.
 * @param transaction The transaction to be enqueued.
 */
void enqueueTransaction(const char* accountNumber, Transaction transaction);

/**
 * Dequeues a transaction from the transaction queue of the specified account.
 * Removes the transaction from the front of the queue and returns it.
 * 
 * @param accountNumber The account number associated with the transaction queue.
 * @return The dequeued Transaction, or NULL if the queue is empty or does not exist.
 */
Transaction* dequeueTransaction(const char* accountNumber);

/**
 * Prints the details of all active user transaction queues.
 * This includes account numbers and the transactions in each queue.
 */
void printUserQueues();

#endif // USER_QUEUE_H
