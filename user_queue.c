#include "user_queue.h"
#include "transactions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

TransactionQueue userQueues[MAX_USERS];


int userCount = 0;
// Modified to only handle the enqueueing of a Transaction.
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

        // Initialize the new queue
        queue = &userQueues[userCount]; // Use the current count for the index
        strcpy(queue->accountNumber, accountNumber);
        queue->front = queue->rear = NULL;
        
        // Increment userCount after the new queue is initialized
        userCount++; // This is where we increment userCount, after the queue setup is complete
        printf("New queue added, userCount incremented to: %d\n", userCount);
        queue->isActive = 1;
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

    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}


Transaction* dequeueTransaction(const char* accountNumber) {
    //printf("Prepping to dequeue transaction for %s\n", accountNumber);
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userQueues[i].accountNumber, accountNumber) == 0) {
            if (userQueues[i].front == NULL) {
                return NULL;  // No transactions to dequeue
            }

            QueueNode *node = userQueues[i].front;
            Transaction* transaction = malloc(sizeof(Transaction));
            if (transaction == NULL) {
                fprintf(stderr, "Memory allocation failed for transaction.\n");
                return NULL;
            }
            *transaction = node->transaction;

            userQueues[i].front = node->next;
            if (userQueues[i].front == NULL) {
                userQueues[i].rear = NULL;  // The queue is empty now
            }

            free(node);
            //printf("Dequeued transaction: %d for %s.\n", transaction->transactionType, transaction->accountNumber);
            return transaction;
        }
    }
    return NULL;  // Account number not found
}

void printUserQueues() {
    printf("Current user queues and their transactions:\n");
    for (int i = 0; i < userCount; i++) {
        if (!userQueues[i].isActive) {
            continue; // Skip inactive users
        }

        printf("User: %s\n", userQueues[i].accountNumber);
        QueueNode* node = userQueues[i].front;
        while (node != NULL) {
            const char* transTypeStr = getTransactionTypeString(node->transaction.transactionType);
            if ((node->transaction.amount == 0.0) && 
                (strcmp(transTypeStr, "INQUIRY") == 0 || strcmp(transTypeStr, "CLOSE") == 0)) {
                printf("\tTransaction Type: %s\n", transTypeStr);
            } else {
                printf("\tTransaction Type: %s, Amount: %.2f\n", transTypeStr, node->transaction.amount);
            }
            node = node->next;
        }
    }
}



