#include "synchronization.h"
#include "transactions.h"
#include "user_queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define NUM_ACCOUNTS 2
#define NUM_TRANSACTIONS 3

void populateUserQueue(Account** acc) {

    enqueueTransaction("123456", (Transaction){CREATE, "123456", acc[0], 1000.0});
    enqueueTransaction("123456", (Transaction){DEPOSIT, "123456", acc[0], 200.0});
    enqueueTransaction("123456", (Transaction){INQUIRY, "123456", acc[0], 0.0}); //testing
    enqueueTransaction("123456", (Transaction){WITHDRAW, "123456", acc[0], 150.0});
    enqueueTransaction("789012", (Transaction){CREATE, "789012", acc[1], 500.0});
    enqueueTransaction("789012", (Transaction){DEPOSIT, "789012", acc[1], 100.0});
    enqueueTransaction("789012", (Transaction){INQUIRY, "789012", acc[1], 0.0});
    enqueueTransaction("123456", (Transaction){INQUIRY, "123456", acc[0], 0.0});
    enqueueTransaction("123456", (Transaction){CLOSE, "123456", acc[0], 0.0});
}

void* threadProcessTransactions(void* arg) {
    TransactionQueue* queue = (TransactionQueue*) arg;
    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    while (transaction != NULL) {
        //enterMonitor(transaction);
        printf("Processing transaction for account: %s of type: %d\n", transaction->accountNumber, transaction->transactionType);
        processTransaction(transaction->account, transaction);
        //free(transaction);
        transaction = dequeueTransaction(queue->accountNumber);
    }
    return NULL;
}

void processAllTransactionsConcurrently() {
    pthread_t threads[userCount];
    for (int i = 0; i < userCount; i++) {
        printf("%s\n", userQueues[i].accountNumber);
        pthread_create(&threads[i], NULL, threadProcessTransactions, &userQueues[i]);
    }
    for (int i = 0; i < userCount; i++) {
        pthread_join(threads[i], NULL);
    }
}

void listQueue(){
    TransactionQueue* queue = &userQueues[0];
    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    while (transaction != NULL) {
        printf("Start\n");
        printf("Account: %s ", transaction->accountNumber);
        printf("Type: %d\n", transaction->transactionType);
        transaction = dequeueTransaction(queue->accountNumber);
        printf("Moving to next.\n");
    }
}



int main() {

    pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mt1 = PTHREAD_MUTEX_INITIALIZER;
    if(pthread_mutex_init(&mt, NULL) != 0) return 1; // kill if broken
    if(pthread_mutex_init(&mt1, NULL) != 0) return 1; // kill if broken

    Account listAccounts0 = {"123456", 0.0, mt};
    Account listAccounts1 = {"789012", 0.0, mt1};
    
    Account* listAccounts[2] = {&listAccounts0, &listAccounts1};

    initMonitor();

    populateUserQueue(listAccounts);

    //listQueue();

    processAllTransactionsConcurrently();


    return 0;
}

