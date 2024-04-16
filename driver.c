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
    enqueueTransaction("123456", (Transaction){INQUIRY, "123456", acc[0], 0.0});
    enqueueTransaction("123456", (Transaction){WITHDRAW, "123456", acc[0], 150.0});
    enqueueTransaction("789012", (Transaction){CREATE, "789012", acc[1], 500.0});
    enqueueTransaction("789012", (Transaction){TRANSFER, "789012", acc[1], 100.0, "123456"});
    enqueueTransaction("789012", (Transaction){DEPOSIT, "789012", acc[1], 100.0});
    enqueueTransaction("789012", (Transaction){INQUIRY, "789012", acc[1], 0.0});
    enqueueTransaction("123456", (Transaction){INQUIRY, "123456", acc[0], 0.0});
    enqueueTransaction("789012", (Transaction){DEPOSIT, "789012", acc[1], 100.0});
    enqueueTransaction("123456", (Transaction){CLOSE, "123456", acc[0], 0.0});
    enqueueTransaction("789012", (Transaction){CLOSE, "789012", acc[0], 0.0});
    enqueueTransaction("123456", (Transaction){INQUIRY, "123456", acc[0], 0.0}); //testing
}

void* threadProcessTransactions(void* arg) {
    TransactionQueue* queue = (TransactionQueue*) arg;
    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    while (transaction != NULL) {
        //enterMonitor(transaction);
        //printf("Processing transaction for account: %s of type: %d\n", transaction->accountNumber, transaction->transactionType); //Debug print
        processTransaction(transaction->account, transaction);
        free(transaction);
        transaction = dequeueTransaction(queue->accountNumber);
    }
    return NULL;
}

void processAllTransactionsConcurrently() {
    pthread_t threads[userCount];
    for (int i = 0; i < userCount; i++) {
        //printf("%s\n", userQueues[i].accountNumber); //Debug print
        pthread_create(&threads[i], NULL, threadProcessTransactions, &userQueues[i]);
    }
    for (int i = 0; i < userCount; i++) {
        pthread_join(threads[i], NULL);
    }
}

//was used for testing
void listQueue(){
    TransactionQueue* queue = &userQueues[0];
    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    while (transaction != NULL) {
        transaction = dequeueTransaction(queue->accountNumber);
    }
}



int main() {

    //pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;
    //pthread_mutex_t mt1 = PTHREAD_MUTEX_INITIALIZER;
    //if(pthread_mutex_init(&mt, NULL) != 0) return 1; // kill if broken
    //if(pthread_mutex_init(&mt1, NULL) != 0) return 1; // kill if broken

    //get num users from inputfile
    int numUsers = 2;
    struct Account* users = (struct Account*)malloc(numUsers * sizeof(*users)); 
  
    // Check for malloc Failure 
    if (users == NULL) { 
        fprintf(stderr, "Memory allocation failed\n"); 
        return 1; 
    } 

    for (int i = 0; i < numUsers; i++) { 
        snprintf(users[i].accountNumber, "%d", i + 1); 
        users[i].balance = 0.0;
        users[i].lock = PTHREAD_MUTEX_INITIALIZER;
        users[i].closed = true;
    }

    snprintf(users[0].accountNumber, "123456"); //, sizeof(users[i].accountNumber)
    snprintf(users[1].accountNumber, "789012"); 

    //Account listAccounts0 = {"123456", 0.0, PTHREAD_MUTEX_INITIALIZER, initial};
    //Account listAccounts1 = {"789012", 0.0, PTHREAD_MUTEX_INITIALIZER, initial};
    
    //Accounts* listAccounts = {&listAccounts0, &listAccounts1};

    initMonitor();

    populateUserQueue(users);

    processAllTransactionsConcurrently();

    free(users);


    return 0;
}

