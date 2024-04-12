#include "synchronization.h"
#include "transactions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

void enterMonitor(MonitorQueue* q, Transaction* t) {
    pthread_mutex_lock(&q->MQlock);  // Lock the entire queue first

    // Dynamically create a new queue node
    QueueNode* newNode = malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        fprintf(stderr, "Failed to allocate memory for a new queue node.\n");
        pthread_mutex_unlock(&q->MQlock);
        return;
    }
    newNode->pid = getpid();
    newNode->next = NULL;

    // Add the new node to the queue
    if (q->rear == NULL) {  // If the queue is empty
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    // Wait until this node is at the front of the queue
    while (q->front != newNode) {
        pthread_cond_wait(&q->cond, &q->MQlock);
    }

    // Perform the operation associated with the transaction
    pthread_mutex_t *selectedLock = &q->lock0; // Default to lock0
    switch (t->transactionType) {
        case CREATE: selectedLock = &q->lock0; break;
        case DEPOSIT: selectedLock = &q->lock1; break;
        case WITHDRAW: selectedLock = &q->lock2; break;
        case INQUIRY: selectedLock = &q->lock3; break;
        case TRANSFER: selectedLock = &q->lock4; break;
        case CLOSE: selectedLock = &q->lock5; break;
        default: fprintf(stderr, "Invalid transaction type.\n");
    }

    pthread_mutex_lock(selectedLock);
    grabWorker(t); // Process the transaction
    pthread_mutex_unlock(selectedLock);

    // Remove this node from the queue
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;  // If the queue is now empty
    }
    free(newNode); // Free the node

    pthread_cond_broadcast(&q->cond); // Notify others waiting
    pthread_mutex_unlock(&q->MQlock); // Unlock the monitor queue
}
void exitMonitor(MonitorQueue* q){
    pthread_mutex_lock(&q->MQlock); // lock queue to remove self from front

    //printf("removing self from queue: %d\n", getpid());

    QueueNode* nextNode = q->front->next;

    if(nextNode == NULL){
        q->front == NULL; // if only one in queue empty queue
        q->rear = q->front;
    }else{
        q->front->next = NULL; // else pop and move to next
        q->front = nextNode;
    }

    

    pthread_mutex_unlock(&q->MQlock); // unlock queue

    //printf("signaling: %d\n", getpid());
    pthread_cond_signal(&q->cond); // signal that queue has been updated
}

void* grabWorker(void* arg) {
    Transaction* transaction = (Transaction*) arg;
    printf("Processing transaction for account: %s, Type: %s, Amount: %.2f\n",
       transaction->accountNumber,
       getTransactionTypeString(transaction->transactionType),
       transaction->amount);

    appendTransactionToFile(transaction);
    return NULL;
}