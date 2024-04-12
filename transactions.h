/*
* Group G
* Abhiram Reddy Alugula
* aalugul@okstate.edu
* 4/7/2024
*/

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

// typedef enum{
//     CREATE, DEPOSIT, WITHDRAW,
//     INQUIRY, TRANSFER, CLOSE
// } transType;

// typedef struct {
//     transType transactionType;
//     char accountNumber[20];
//     double amount;
//     char timestamp[20];
//     char recipientAccountNumber[20];
//     struct Transaction* next;
// } Transaction;


// void appendTransactionToFile(Transaction* transaction);

// #endif

typedef enum {
    WITHDRAW, CREATE, DEPOSIT, INQUIRY, TRANSFER, CLOSE
} TransType;

typedef struct {
    TransType type;
    char accountNumber[20];
    double amount;
    char recipientAccountNumber[20];
} Transaction;

void* process_transaction(void* arg);

#endif