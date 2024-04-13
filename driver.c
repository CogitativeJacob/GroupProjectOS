#include "transactions.h"
#include "user_queue.h"
#include <pthread.h>
#include <stdio.h>
#define NUM_ACCOUNTS 2
#define NUM_TRANSACTIONS 3

void populateUserQueue() {
    enqueueTransaction("123456", (Transaction){CREATE, "123456", 1000.0});
    enqueueTransaction("123456", (Transaction){DEPOSIT, "123456", 200.0});
    enqueueTransaction("123456", (Transaction){WITHDRAW, "123456", 150.0});
    enqueueTransaction("789012", (Transaction){CREATE, "789012", 500.0});
    enqueueTransaction("789012", (Transaction){DEPOSIT, "789012", 100.0});
    enqueueTransaction("789012", (Transaction){INQUIRY, "789012", 0.0});
    enqueueTransaction("123456", (Transaction){INQUIRY, "123456", 0.0});
    enqueueTransaction("123456", (Transaction){CLOSE, "123456", 0.0});
}

void* threadProcessTransactions(void* arg) {
    TransactionQueue* queue = (TransactionQueue*) arg;
    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    while (transaction != NULL) {
        //enterMonitor(transaction);
        processTransaction(queue->accountNumber, transaction);
        free(transaction);
        transaction = dequeueTransaction(queue->accountNumber);
    }
    return NULL;
}

void processAllTransactionsConcurrently() {
    pthread_t threads[userCount];
    for (int i = 0; i < userCount; i++) {
        pthread_create(&threads[i], NULL, threadProcessTransactions, &userQueues[i]);
    }
    for (int i = 0; i < userCount; i++) {
        pthread_join(threads[i], NULL);
    }
}



int main() {

    initMonitor();

    populateUserQueue();

    processAllTransactionsConcurrently();


    return 0;
}

