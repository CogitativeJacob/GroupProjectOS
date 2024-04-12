/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/7/2024
*/

#include "transactions.h"
#include "synchronization.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"
#include <pthread.h>


MonitorQueue mq = {.front = NULL, .rear = NULL, .lock0 = PTHREAD_MUTEX_INITIALIZER, .lock1 = PTHREAD_MUTEX_INITIALIZER,
 .lock2 = PTHREAD_MUTEX_INITIALIZER, .lock3 = PTHREAD_MUTEX_INITIALIZER, .lock4 = PTHREAD_MUTEX_INITIALIZER,
 .lock5 = PTHREAD_MUTEX_INITIALIZER, .MQlock = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER};

void processUserTransactions(const char* accountNumber) { //Not fully finished yet (waiting for monitor development)
    // Assuming transactions for this user have been enqueued
    // This function simulates processing those transactions in a separate process
    //printf("Processing transactions for account: %s\n", accountNumber);
    Transaction* transaction;
    while ((transaction = dequeueTransaction(accountNumber)) != NULL) {
        enterMonitor(&mq, transaction); // Synchronize access
        printf("Processed transaction for %s: Type %d, Amount %.2f\n",
               accountNumber, transaction->transactionType, transaction->amount);
        exitMonitor(&mq);
        free(transaction); // Assume dynamically allocated
    }
}


int main() {
    //Test transactions for monitor implementation
    Transaction transaction[] = {
    {DEPOSIT, "123456789", 100.0, "2023-03-26"},
    {DEPOSIT, "123456789", 50.0, "2023-03-27"},
    {INQUIRY, "123456789", 0, "2023-03-28"},
    {TRANSFER, "123456789", 100.0, "2023-03-29"}};

    parseAndEnqueueTransactions("/home/burger/Desktop/GroupProjectOS/input.txt");
    printUserQueues();
    
    //Testing user queues
    for (int i = 0; i < numUniqueUsers; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            //printf(uniqueUsers[i]);
            processUserTransactions(uniqueUsers[i]);
            exit(0);
        }
    }

    // //Using Monitor
    // pid_t pid;
    // for (int i = 0; i < 4; i++) { // Simulate 5 concurrent transactions
    //     pid = fork();
    //     if (pid == 0) { // Child process

    //         // for threads with multiple transactions we will loop the following
    //         // two lines for each one
    //         enterMonitor(&mq, &transaction[i]); // enter queue for monitor
    //         printf("Transaction %s entered monitor\n", transaction[i].accountNumber);
    //         exitMonitor(&mq); // exit queue for monitor
    //         return 0;
    //     }
    // }

    while (wait(NULL) > 0); // Wait for all child processes to finish

    return 0;
}