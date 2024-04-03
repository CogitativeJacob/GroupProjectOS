#include "user_queue.h"
#include "transactions.h"
#include "synchronization.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"
#include <pthread.h>


MonitorQueue mq = {.front = NULL, .rear = NULL, .lock = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER};

void processUserTransactions(const char* accountNumber) { //Not fully finished yet (waiting for monitor development)
    // Assuming transactions for this user have been enqueued
    // This function simulates processing those transactions in a separate process
    printf("Processing transactions for account: %s\n", accountNumber);
    Transaction* transaction;
    while ((transaction = dequeueTransaction(accountNumber)) != NULL) {
        //enterMonitor(&mq, transaction); // Synchronize access
        printf("Processed transaction for %s: Type %d, Amount %.2f\n",
               accountNumber, transaction->transactionType, transaction->amount);
        //exitMonitor(&mq);
        free(transaction); // Assume dynamically allocated
    }
}

int main() {
    
    parseAndEnqueueTransactions("/home/burger/Desktop/GroupProjectOS/input.txt");

    printUserQueues();

    // In your main driver file
    for (int i = 0; i < numUniqueUsers; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            printf(uniqueUsers[i]);
            processUserTransactions(uniqueUsers[i]);
            exit(0);
        }
    }

    // Parent waits for all child processes to finish
    while (wait(NULL) > 0);

    printUserQueues();

    return 0;
}
