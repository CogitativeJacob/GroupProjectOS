/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/7/2024
*/

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

typedef enum{
    CREATE, DEPOSIT, WITHDRAW,
    INQUIRY, TRANSFER, CLOSE
} transType;

typedef struct {
    transType transactionType;
    char accountNumber[20];
    double amount;
    char timestamp[20];
    char recipientAccountNumber[20];
    struct Transaction* next;
} Transaction;


void appendTransactionToFile(Transaction* transaction);

#endif
