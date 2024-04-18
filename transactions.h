/*
* Group G
* Abhiram Reddy Alugula
* aalugul@okstate.edu
* 4/7/2024
*/

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHM_KEY 0x1234
#define MAX_TRANSACTIONS 10

typedef enum {
    CREATE, DEPOSIT, WITHDRAW, INQUIRY, TRANSFER, CLOSE, FAILED
} TransactionType;

typedef struct {
    TransactionType type;
    char accountNumber[20];
    double amount;
    char recipientAccountNumber[20]; // Used for transfers
    char status[8]; // "success" or "failed"
    char timestamp[20]; // Storing timestamp as string
    int processed; // Flag to indicate if processed (1 = yes, 0 = no)
} SharedTransaction;

#endif
