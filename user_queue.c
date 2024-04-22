/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/21/2024
*/

#include "user_queue.h"
#include "transactions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

TransactionQueue userQueues[MAX_USERS]; // Global array to manage transaction queues for each user

int userCount = 0; // Global counter to track the number of active users

/**
 * Adds a transaction to the queue associated with a specific account number.
 * If the queue does not exist, it creates a new one.
 * 
 * @param accountNumber The account number associated with the transaction.
 * @param transaction The transaction to enqueue.
 */
void enqueueTransaction(const char* accountNumber, Transaction transaction) {
    int found = -1;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userQueues[i].accountNumber, accountNumber) == 0) {
            found = i;
            break;
        }
    }

    TransactionQueue* queue;
    if (found == -1) {
        if (userCount >= MAX_USERS) {
            fprintf(stderr, "Maximum number of users reached.\n");
            return;
        }
        queue = &userQueues[userCount]; // Initialize the new queue at the current userCount index
        strcpy(queue->accountNumber, accountNumber);
        queue->front = queue->rear = NULL;
        userCount++; // Increment user count after initializing the queue
        queue->isActive = 1;
        printf("New queue added, userCount incremented to: %d\n", userCount);
    } else {
        queue = &userQueues[found];
    }

    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed for new transaction node.\n");
        return;
    }
    newNode->transaction = transaction;
    newNode->next = NULL;

    if (queue->rear == NULL) { // Queue is empty
        queue->front = queue->rear = newNode; // New node is now the first node
    } else {
        queue->rear->next = newNode; // Append new node at the end
        queue->rear = newNode;
    }
}

/**
 * Removes and returns the first transaction from the queue of a specific account.
 * 
 * @param accountNumber The account number whose transaction queue is to be dequeued.
 * @return A pointer to the dequeued Transaction, or NULL if the queue is empty or not found.
 */
Transaction* dequeueTransaction(const char* accountNumber) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userQueues[i].accountNumber, accountNumber) == 0) {
            if (userQueues[i].front == NULL) {
                return NULL; // No transactions to dequeue
            }

            QueueNode *node = userQueues[i].front;
            Transaction* transaction = malloc(sizeof(Transaction));
            if (transaction == NULL) {
                fprintf(stderr, "Memory allocation failed for transaction.\n");
                return NULL;
            }
            *transaction = node->transaction; // Copy the transaction data

            userQueues[i].front = node->next; // Move front pointer to next node
            if (userQueues[i].front == NULL) {
                userQueues[i].rear = NULL; // Queue is now empty
            }

            free(node); // Free the dequeued node
            return transaction;
        }
    }
    return NULL; // Account number not found
}

/**
 * Prints the transaction queues of all active users.
 */
void printUserQueues() {
    printf("Current user queues and their transactions:\n");
    for (int i = 0; i < userCount; i++) {
        if (!userQueues[i].isActive) {
            continue; // Skip inactive queues
        }

        printf("User: %s\n", userQueues[i].accountNumber);
        QueueNode* node = userQueues[i].front;
        while (node != NULL) {
            const char* transTypeStr = getTransactionTypeString(node->transaction.transactionType);
            if ((node->transaction.amount == 0.0) && 
                (strcmp(transTypeStr, "INQUIRY") == 0 || strcmp(transTypeStr, "CLOSE") == 0)) {
                printf("\tTransaction Type: %s\n", transTypeStr); // Print without amount
            } else {
                printf("\tTransaction Type: %s, Amount: %.2f\n", transTypeStr, node->transaction.amount);
            }
            node = node->next;
        }
    }
}
