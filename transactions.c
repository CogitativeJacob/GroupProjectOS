/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/7/2024
*/

#include "transactions.h"
#include <stdio.h>
#include <pthread.h>
#include <time.h>

// Global mutex for file access synchronization
//pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

void appendTransactionToFile(Transaction* transaction) {
    //pthread_mutex_lock(&fileMutex); // Lock the mutex before accessing the file
    //Need to use shared memory here

    FILE* file = fopen("log.txt", "a"); // Open the file in append mode
    if (file != NULL) {
        fprintf(file, "Type: %d, Account: %s, Amount: %.2f, Timestamp: %s\n",
                transaction->transactionType, transaction->accountNumber,
                transaction->amount, transaction->timestamp);
        fclose(file); // Close the file after writing
    } else {
        perror("Failed to open transaction log file");
    }

    //pthread_mutex_unlock(&fileMutex); // Unlock the mutex after file operation is done
}
